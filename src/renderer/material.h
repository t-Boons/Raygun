#pragma once
#include "precomp.h"
#include "renderer/texture.h"

enum class MaterialType
{
	LambertDiffuse, Glass, Mirror, Glossy, Emissive
};

class Texture;
struct Material
{
	float3 color{ 0 };
	float specularity = 0;
	float roughness = 0;
	Texture* texture = nullptr;
	float refractance = 1.0f;
	MaterialType type = MaterialType::LambertDiffuse;

	~Material()
	{
		delete texture;
	}
};