#pragma once
#include <precomp.h>

struct SceneData
{
	SceneData(uint32_t seed)
		: random(seed)
	{}

	float3 normal{ 0 };
	Random random;
	float currentX, currentY;
	uint32_t bounce = 0;
};