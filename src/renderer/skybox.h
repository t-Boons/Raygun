#pragma once
#include "settings.h"

class Skybox
{
public:
	Skybox(const char* filePath);

	inline float3 GetSample(const float3& dir)
	{
		if (Settings::bilinearInterpolation)
		{
			const float3 ndir = normalize(dir);

			// Calculate uv coordinate in the texture space.
			const float theta = fastacos(ndir.y);
			const float phi = fastatan2(ndir.x, ndir.z) + rotation;

			const float u = (phi / TWOPI + 0.5f);
			const float v = (1.0f - theta * INVPI);

			return bilinearInterp(texture, u, v, width, height, channels) * intensity;
		}
		else
		{
			// Sample directly from the rounded pixel value.
			const float theta = fastacos(dir.y);
			const int y = clamp((int)((float)(1.0f - theta * INVPI) * height), 0, height - 1) * channels;

			const float phi = fastatan2(dir.x, dir.z) + rotation;
			const int x = clamp((int)((float)(phi / TWOPI + 0.5f) * width), 0, width - 1) * channels;

			const float* tex = &texture[y * width + x];

			return make_float3(*tex, *(tex + 1), *(tex + 2)) * intensity;
		}
	}

public:
	float2 pixelSize;
	float intensity = 1.0f;
	float* texture;
	int width;
	int height;
	int channels;
	float rotation;
};