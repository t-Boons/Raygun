#include "precomp.h"
#include "renderer/texture.h"
#include <omp.h>
#include "gameplay/Game.h"

void Renderer::ResetAccumulator()
{
	frameCount = 1;
	memset(accumulator, 0, sizeof(float4) * SCRWIDTH * SCRHEIGHT);
	memset(normals, 0, sizeof(float4) * SCRWIDTH * SCRHEIGHT);
	memset(accumulatorFiltered, 0, sizeof(float4) * SCRWIDTH * SCRHEIGHT);
}

void Renderer::Init()
{
	// create fp32 rgb pixel buffer to render to
	accumulator = (float4*)MALLOC64(sizeof(float4) * SCRWIDTH * SCRHEIGHT);
	normals = (float4*)MALLOC64(sizeof(float4) * SCRWIDTH * SCRHEIGHT);
	accumulatorFiltered = (float4*)MALLOC64(sizeof(float4) * SCRWIDTH * SCRHEIGHT);

	ResetAccumulator();

	denoiser.Init(SCRWIDTH, SCRHEIGHT);
}

float3 Renderer::CalculatePointLight(const PointLight& light, const float3& I, const float3& N)
{
	// Calculate the point light.
	const float3 dir = normalize(light.position - I);
	const float diffuse = dot(dir, N);
	if (diffuse < 0) return 0;

	const float rayLength = length(light.position - I);
	const float3 att = attenuation(light.color, rayLength / light.radius);
	if (att.x <= 0 && att.y <= 0 && att.z <= 0) return 0;

	const Ray shadowRay(I + dir * epsilon, dir, rayLength);

	if (!scene.IsOccluded(shadowRay))
	{
		return att * light.color * diffuse;
	}

	return 0;
}

float3 Renderer::CalculateSpotLight(const SpotLight& light, const float3& I, const float3& N)
{
	// Calculate the point light.
	const float3 dir = normalize(light.position - I);
	const float diffuse = dot(dir, N);
	if (diffuse < 0) return 0;

	const float rayLength = length(light.position - I);
	const float3 att = attenuation(light.color, rayLength * light.radius);
	if (att.x <= 0 && att.y <= 0 && att.z <= 0) return 0;

	// https://learnopengl.com/Lighting/Light-casters
	const float theta = dot(dir, normalize(-light.direction));
	const float e = light.cutoff - light.outerCutoff;
	const float intensity = 1.0f - clamp((theta - light.outerCutoff) / e, 0.0f, 1.0f);
	if (intensity <= 0.0f) return 0;

	const Ray shadowRay(I + dir * epsilon, dir, rayLength);

	if (!scene.IsOccluded(shadowRay))
	{
		return light.color * intensity * att * diffuse;
	}

	return 0;
}

float3 Renderer::CalculateDirectionalLight(const DirectionalLight& light, const float3& I, const float3& N)
{
	const float3 dir = normalize(-light.direction);
	const float diffuse = dot(dir, N);
	if (diffuse < 0.0f) return 0;

	const Ray shadowRay(I + dir * epsilon, dir);

	if (!scene.IsOccluded(shadowRay))
	{
		return light.color * diffuse;
	}

	return 0;
}

float3 Renderer::CalculateAreaLight(const AreaLight& light, const float3& I, const float3& N, Random& rand)
{
	static const float numSamples = 1;

	// Calculate area lights using monte carlo integration.
	float3 totalIllum = 0.0f;

	for (int x = 0; x < numSamples; ++x)
	{
		const float3 point = samplePointOnAreaLight(light, rand);

		const float3 dir = normalize(point - I);
		const float diffuse = dot(dir, N);
		if (diffuse < 0) return 0;

		const float distanceToLight = length(I - point);
		const float3 att = light.color * (light.width * light.height) / (distanceToLight * distanceToLight);
		if (att.x <= epsilon && att.y <= epsilon && att.z <= epsilon) continue;

		const Ray shadowRay(I + dir * epsilon, dir, distanceToLight);

		if (!scene.IsOccluded(shadowRay))
		{
			totalIllum += light.color * att * diffuse;
		}

		return totalIllum / numSamples;
	}

	return 0;
}

float3 Tmpl8::Renderer::GetDirectIllumination(const float3& I, const float3& N, Random& rand)
{
	float3 out(0);

	if (Settings::stochasticLights)
	{
		if (scene.pointLights.size() > 0)
		{
			out += CalculatePointLight(scene.pointLights[rand.RandomUInt() % scene.pointLights.size()], I, N)
				* (float)scene.pointLights.size();
		}
		if (scene.directionalLights.size() > 0)
		{
			out += CalculateDirectionalLight(scene.directionalLights[rand.RandomUInt() % scene.directionalLights.size()], I, N)
				* (float)scene.directionalLights.size();
		}

		if (scene.spotlights.size() > 0)
		{
			out += CalculateSpotLight(scene.spotlights[rand.RandomUInt() % scene.spotlights.size()], I, N)
				* (float)scene.spotlights.size();
		}

		if (scene.areaLights.size() > 0)
		{
			out += CalculateAreaLight(scene.areaLights[rand.RandomUInt() % scene.areaLights.size()], I, N, rand)
				* (float)scene.areaLights.size();
		}
	}
	else
	{
		for(auto& light : scene.pointLights)
		{
			out += CalculatePointLight(light, I, N);
		}
		for (auto& light : scene.directionalLights)
		{
			out += CalculateDirectionalLight(light, I, N);
		}

		for (auto& light : scene.spotlights)
		{
			out += CalculateSpotLight(light, I, N);
		}

		for (auto& light : scene.areaLights)
		{
			out += CalculateAreaLight(light, I, N, rand);
		}
	}
	return out;
}

// -----------------------------------------------------------
// Evaluate light transport
// -----------------------------------------------------------
float3 Renderer::Trace(Ray& ray, SceneData& sceneData)
{
	// Ray trace bounce cap.
	if (sceneData.bounce > Settings::maxBounces) return float3(0);
	
	// If it shoots in to the sky than sample the sky.
	scene.FindNearest(ray);
	if (!ray.object) return scene.skybox.intensity > 0.01f ? scene.skybox.GetSample(ray.D) : 0;

	const mat4& objectMatrix = RenderLevel::transforms[ray.object->matrixIndex].invMatrix;
	const float3 I = ray.O + ray.t * ray.D;

	const float3 tO = TransformPosition(ray.O, objectMatrix);
	const float3 tD = TransformVector(ray.D, objectMatrix);
	const float3 tDsign = (float3(copySign(tD.x), copySign(tD.y), copySign(tD.z)) + 1) * 0.5f;
	const float3 tI = tO + ray.t * tD;
	const float3 tN = getNormal(tI, tDsign, ray.object->type);
	const float3 N = normalize(TransformVector(tN, RenderLevel::transforms[ray.object->matrixIndex].matrix));

	if(sceneData.bounce == 0) sceneData.normal = N;

	const Material& mat = scene.materials[ray.object->type == PrimitiveType::Voxel ? ray.object->grid[ray.voxelIndex] : ray.object->materialIndex];
	const float3 light = GetDirectIllumination(I, N, sceneData.random);

	// Get material color.
	float3 color = mat.color;
	if (mat.texture)
	{
		const float2 uv = getUV(tI, tN, ray.object->type);
		if (Settings::mipmapping)
		{
			// sceneData.currentX + 0.1f I have NO CLUE why it looks better if I add only 0.1 pixels instead of 1.
			// This works for now.
			Ray differencialRay(camera.GetPrimaryRay(sceneData.currentX + 0.1f, sceneData.currentY));
			scene.FindNearest(differencialRay);

			if (differencialRay.object != ray.object || 
				(differencialRay.object->type == PrimitiveType::Voxel && differencialRay.voxelIndex != ray.voxelIndex))
			{
				color = mat.texture->GetPixel(uv.x, uv.y, 0);
			}
			else
			{
				Ray tDifRay = Ray(TransformPosition(differencialRay.O, objectMatrix),
					TransformVector(differencialRay.D, objectMatrix));
				tDifRay.t = differencialRay.t;

				const float3 tDifI = tDifRay.O + tDifRay.D * tDifRay.t;
				const float2 uv2 = getUV(tDifI, getNormal(tDifI, tDifRay.Dsign, ray.object->type), differencialRay.object->type);
				const float differencial = abs(uv.x - uv2.x) * ((float)mat.texture->GetWidth());

				color = mat.texture->GetPixel(uv.x, uv.y, clamp((int)differencial, 0, mat.texture->NumMipMaps() - 1));
			}
		}
		else
		{
			color = mat.texture->GetPixel(uv.x, uv.y, 0);
		}
	}

	if (mat.type == MaterialType::Glossy)
	{
		// Glossy materials.
		const float3 R = reflect(ray.D, N + randomFloat3(-0.5f, 0.5f, sceneData.random) * mat.roughness);
		sceneData.bounce++;
		Ray r(I + R * epsilon, R);
		return color * (mat.specularity * Trace(r, sceneData)) + light;
	}

	if (mat.type == MaterialType::Emissive)
	{
		return mat.color + light;
	}

	if (mat.type == MaterialType::LambertDiffuse)
	{
		const float3 R = randomPointOnHemisphere(N, sceneData.random);
		const float diffuse = dot(R, N);

		Ray r(I + R * epsilon, R);	
		sceneData.bounce++;
		const float3 irradiance = Trace(r, sceneData) * diffuse;
		return color * irradiance + color * light;
	}

	// Reflection.
	if (mat.type == MaterialType::Mirror)
	{
		const float3 R = reflect(ray.D, N);

		sceneData.bounce++;
		Ray r(I + R * epsilon, R);
		return color * (mat.specularity * Trace(r, sceneData)) + (1.0f - mat.specularity) * light;
	}

	// Refraction
	if (mat.type == MaterialType::Glass)
	{
		// Code is inspired by raytracing in 1 weekend but haevily modified.

		// Calculate ior.
		const float ior = dot(ray.D, N) ? (1.0f / mat.refractance) : mat.refractance;

		// Calculate how much the ray should refrace and reflect.
		const float cosTheta = fmin(dot(-ray.D, N), 1.0f);
		const float sinTheta = sqrtf(1.0f - cosTheta * cosTheta);

		const bool cannot_refract = ior * sinTheta > 1.0;
		const bool reflectOrRefract = cannot_refract || fresnelSchlick(cosTheta, ior) > sceneData.random.Float();

		// Calculate the ray needed for absorbtion.
		const float3 refractDir = refract(ray.D, N, ior);
		Ray absorbtionRay(I + ray.D * epsilon, refractDir);
		scene.FindExit(absorbtionRay);

		sceneData.bounce++;

		// Decide to shoot a reflection or refraction ray.
		float3 col;
		if (reflectOrRefract)
		{
			const float3 reflectDir = reflect(ray.D, N);
			Ray r(I - ray.D * epsilon, reflectDir);
			col = Trace(r, sceneData);
		}
		else
		{
			const float3 exit = absorbtionRay.O + absorbtionRay.D * (absorbtionRay.t + epsilon);
			Ray r(exit, refractDir);
			col = Trace(r, sceneData);

			// Apply absorbtion.
			col.x *= exp(color.x * absorbtionRay.t);
			col.y *= exp(color.y * absorbtionRay.t);
			col.z *= exp(color.z * absorbtionRay.t);
		}

		return col;
	}

	return float3(0.0f);
}

void Renderer::Tick(float deltaTime)
{
	Timer t;
	// If moving reset the accumulator.
	if (camera.HandleInput(deltaTime))
	{
		ResetAccumulator();
	}

	const float frameRecipracle = 1.0f / (float)frameCount;

	// Calculate which quadrant on a pixel it should sample from.
	float2 quadrant{};
	const int quadrantCount = frameCount % 4;
	if (quadrantCount == 0) quadrant = { -0.5f,  -0.5f };
	if (quadrantCount == 1) quadrant = { 0.5f,  -0.5f };
	if (quadrantCount == 2) quadrant = { 0.5f,   0.5f };
	if (quadrantCount == 3) quadrant = { -0.5f,   0.5f };

	// lines are executed as OpenMP parallel tasks (disabled in DEBUG)

#pragma omp parallel for schedule(dynamic)
	for (int y = 0; y < SCRHEIGHT; ++y)
	{
		const uint32_t threadID = (uint32_t)omp_get_thread_num();

		const float fy = (float)y;
		const int ys = y * SCRWIDTH;

		// trace a primary ray for each pixel on the line
		for (int x = 0; x < SCRWIDTH; ++x)
		{
			float3 color(0);

			// Generate seed from thread number.
			SceneData data{ threadSeed + threadID * 129045 };
			threadSeed++;

			if (Settings::multisampled)
			{
				const float multiSampledX = (float)x + data.random.Float() * quadrant.x;
				const float multiSampledY = fy + data.random.Float() * quadrant.y;

				data.currentX = multiSampledX;
				data.currentY = multiSampledY;
				Ray r(camera.GetPrimaryRay(multiSampledX, multiSampledY));
				color += (Trace(r, data));
			}
			else
			{
				data.currentX = (float)x;
				data.currentY = (float)y;
				Ray r(camera.GetPrimaryRay((float)x, fy));
				color += (Trace(r, data));
			}

			normals[x + ys] = data.normal;

			// Clamping used to supress fireflies.
			static const float max = 5.0f;
			if (dot(color, color) > 25) color = 5 * normalize(color);

			accumulator[x + ys] += color;
		}
	}
	frameCount++;

	if (Settings::denoise)
	{
		if (frameCount <= 1) denoiser.SetComparer(normals);
		denoiser.Filter(accumulator, accumulatorFiltered);

#pragma omp parallel for schedule(dynamic)
		for (int y = 0; y < SCRHEIGHT; ++y)
		{
			const int ys = y * SCRWIDTH;

			for (int x = 0; x < SCRWIDTH; ++x)
			{
				const float4 out = accumulatorFiltered[x + ys] * frameRecipracle;
				screen->pixels[x + ys] = RGBF32_to_RGB8(&out);
			}
		}
	}
	else
	{
#pragma omp parallel for schedule(dynamic)
		for (int y = 0; y < SCRHEIGHT; ++y)
		{
			const int ys = y * SCRWIDTH;

			for (int x = 0; x < SCRWIDTH; ++x)
			{
				const float4 out = accumulator[x + ys] * frameRecipracle;
				screen->pixels[x + ys] = RGBF32_to_RGB8(&out);
			}
		}
	}

	// performance report - running average - ms, MRays/s
	static float avg = 10, alpha = 1;
	avg = (1 - alpha) * avg + alpha * t.elapsed() * 1000;
	if (alpha > 0.05f) alpha *= 0.5f;
	float fps = 1000.0f / avg, rps = (SCRWIDTH * SCRHEIGHT) / avg;

	if (frameCount % 10 == 0)
	{
		printf("%5.2fms (%.1ffps) - %.1fMrays/s\n", avg, fps, rps / 1000);
	}
}

void Tmpl8::Renderer::DrawLine(const float3& start, const float3& end, uint32_t color)
{
	const float2 uv1 = worldSpaceToScreenSpace(start, camera.camPos, camera.ahead, camera.aspect, Settings::fov);
	const float2 uv2 = worldSpaceToScreenSpace(end, camera.camPos, camera.ahead, camera.aspect, Settings::fov);
	screen->Line(uv1.x * SCRWIDTH, uv1.y * SCRHEIGHT, uv2.x * SCRWIDTH, uv2.y * SCRHEIGHT, color);
}
