#pragma once
#include "renderer/lightTransport.h"
#include "renderer/light.h"
#include "renderer/camera.h"
#include "scene.h"
#include "renderer/denoiser.h"

namespace Tmpl8
{
	class Renderer
	{
	public:
		void Init();
		void ResetAccumulator();

		float3 Trace(Ray& ray, SceneData& sceneData);
		void Tick(float deltaTime);

		void DrawLine(const float3& start, const float3& end, uint32_t color);

		Camera* GetCamera() { return &camera; }
		RenderLevel* GetScene() { return &scene; }

		void SetDrawSurface(Surface* s) { screen = s; }

		int NumAccumulatedFrames() const { return frameCount; }

	private:
		float3 CalculatePointLight(const PointLight& light, const float3& I, const float3& N);
		float3 CalculateSpotLight(const SpotLight& light, const float3& I, const float3& N);
		float3 CalculateDirectionalLight(const DirectionalLight& light, const float3& I, const float3& N);
		float3 CalculateAreaLight(const AreaLight& light, const float3& I, const float3& N, Random& rand);
		float3 GetDirectIllumination(const float3& I, const float3& N, Random& rand);

		float4* accumulator;
		float4* accumulatorFiltered;
		float4* normals;
		Surface* screen;

		RenderLevel scene;
		Camera camera;
		Denoiser denoiser;

		int frameCount = 0;
		int threadSeed = 1;

		const float epsilon = 0.001f;
	};

} // namespace Tmpl8