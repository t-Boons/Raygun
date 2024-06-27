#pragma once
#include "renderer/settings.h"
#include "renderer/ray.h"

// default screen resolution
#define SCRWIDTH	512
#define SCRHEIGHT	512
// #define FULLSCREEN
// #define DOUBLESIZE

namespace Tmpl8 {
	
	class Camera
	{
	public:
		Camera()
		{
			// setup a basic view frustum
			camPos = float3(0, 0, -2);
			camTarget = float3(0, 0, -1);
			topLeft = float3(-aspect, 1, 0);
			topRight = float3(aspect, 1, 0);
			bottomLeft = float3(-aspect, -1, 0);
		}
		Ray GetPrimaryRay(const float x, const float y)
		{
			// calculate pixel position on virtual screen plane
			const float u = x * (1.0f / SCRWIDTH);
			const float v = y * (1.0f / SCRHEIGHT);
			const float3 P = topLeft + u * (topRight - topLeft) + v * (bottomLeft - topLeft);
			// return Ray( camPos, normalize( P - camPos ) );
			return Ray(camPos, normalize(P - camPos));
			// Note: no need to normalize primary rays in a pure voxel world
			// TODO:
			// - if we have other primitives as well, we *do* need to normalize!
			// - there are far cooler camera models, e.g. try 'Panini projection'.
		}
		bool HandleInput(const float t)
		{
			if (!WindowHasFocus()) return false;
			float speed = 0.0015f * t;
			ahead = normalize(camTarget - camPos);
			float3 tmpUp(0, 1, 0);
			right = normalize(cross(tmpUp, ahead));
			up = normalize(cross(ahead, right));
			bool changed = false;
			if (IsKeyDown(GLFW_KEY_UP)) camTarget -= speed * up, changed = true;
			if (IsKeyDown(GLFW_KEY_DOWN)) camTarget += speed * up, changed = true;
			if (IsKeyDown(GLFW_KEY_LEFT)) camTarget -= speed * right, changed = true;
			if (IsKeyDown(GLFW_KEY_RIGHT)) camTarget += speed * right, changed = true;
			ahead = normalize(camTarget - camPos);
			right = normalize(cross(tmpUp, ahead));
			up = normalize(cross(ahead, right));
			if (IsKeyDown(GLFW_KEY_A)) camPos -= speed * right, changed = true;
			if (IsKeyDown(GLFW_KEY_D)) camPos += speed * right, changed = true;
			if (GetAsyncKeyState('W')) camPos += speed * ahead, changed = true;
			if (IsKeyDown(GLFW_KEY_S)) camPos -= speed * ahead, changed = true;
			if (IsKeyDown(GLFW_KEY_E)) camPos += speed * up, changed = true;
			if (IsKeyDown(GLFW_KEY_Q)) camPos -= speed * up, changed = true;
			camTarget = camPos + ahead;
			ahead = normalize(camTarget - camPos);
			up = normalize(cross(ahead, right));
			right = normalize(cross(up, ahead));

			float canvasSize = tanf(Settings::fov * 0.017453f);

			topLeft = camPos + canvasSize * ahead - aspect * right + up;
			topRight = camPos + canvasSize * ahead + aspect * right + up;
			bottomLeft = camPos + canvasSize * ahead - aspect * right - up;
			bottomRight = camPos + canvasSize * ahead + aspect * right - up;
			if (!changed) return false;
			return true;
		}
		float aspect = (float)SCRWIDTH / (float)SCRHEIGHT;
		float3 camPos, camTarget;
		float3 topLeft, topRight, bottomLeft, bottomRight, ahead, right, up;
	};

	/*
	class Camera
	{
	public:
		Camera()
		{
			// setup a basic view frustum
			camPos = float3(0, 0, -2);
			camTarget = float3(0, 0, -1);
		}

		Ray GetPrimaryRay(const float x, const float y)
		{

			const float u = (SCRWIDTH - x) / SCRWIDTH * 2.0f - 1.0f;
			const float v = (SCRHEIGHT - y) / SCRHEIGHT * 2.0f - 1.0f;

			const float2 tc(u, v);

			const float fov = Settings::fov * PI;
			const float d = Settings::distortion;

			const float3 panini = paniniProjection(tc, fov, d);

			float3 rd;
			rd.x = panini.x * -right.x + panini.y * -right.y + panini.z * -right.z;
			rd.y = panini.x * up.x + panini.y * up.y + panini.z * up.z;
			rd.z = panini.x * ahead.x + panini.y * ahead.y + panini.z * ahead.z;

			return Ray(camPos, rd);
		}

		bool HandleInput(const float t)
		{
			if (!WindowHasFocus()) return false;
			float speed = 0.0015f * t;
			ahead = normalize(camTarget - camPos);
			const float3 tmpUp(0, 1, 0);
			right = normalize(cross(tmpUp, ahead));
			up = normalize(cross(ahead, right));
			bool changed = false;
			if (IsKeyDown(GLFW_KEY_A)) camPos -= speed * 2 * right, changed = true;
			if (IsKeyDown(GLFW_KEY_D)) camPos += speed * 2 * right, changed = true;
			if (IsKeyDown(GLFW_KEY_W)) camPos += speed * 2 * ahead, changed = true;
			if (IsKeyDown(GLFW_KEY_S)) camPos -= speed * 2 * ahead, changed = true;
			if (IsKeyDown(GLFW_KEY_E)) camPos += speed * 2 * up, changed = true;
			if (IsKeyDown(GLFW_KEY_Q)) camPos -= speed * 2 * up, changed = true;
			camTarget = camPos + ahead;
			if (IsKeyDown(GLFW_KEY_UP)) camTarget -= speed * up, changed = true;
			if (IsKeyDown(GLFW_KEY_DOWN)) camTarget += speed * up, changed = true;
			if (IsKeyDown(GLFW_KEY_LEFT)) camTarget -= speed * right, changed = true;
			if (IsKeyDown(GLFW_KEY_RIGHT)) camTarget += speed * right, changed = true;
			if (!changed) return false;
			return true;
		}

		float aspect = (float)SCRWIDTH / (float)SCRHEIGHT;
		float3 camPos, camTarget, right, up, ahead;
	};*/
}