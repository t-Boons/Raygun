#pragma once
#include "primitives.h"

// Template, IGAD version 3
// IGAD/NHTV/UU - Jacco Bikker - 2006-2022
class ALIGN(64) Ray
{
public:
	Ray() = default;
	Ray(const float3 origin, const float3 direction, const float rayLength = 1e34f)
		: O(origin), D(direction), t(rayLength)
	{
		if (D.x == 0) D.x += 0.000005f;
		if (D.y == 0) D.y += 0.000005f;
		if (D.z == 0) D.z += 0.000005f;

		rD = float3(1 / D.x, 1 / D.y, 1 / D.z);
		Dsign = (float3(copySign(D.x), copySign(D.y), copySign(D.z)) + 1) * 0.5f;
	}

	float3 O;										    // 12
	float3 rD;										    // 12
	float3 D = float3(0);							    // 12
	float3 Dsign = float3(1);						    // 12
	float t = 1e34f;									// 4
	uint32_t voxelIndex = 0;							// 4
	const Primitive* object = nullptr;					// 8
};