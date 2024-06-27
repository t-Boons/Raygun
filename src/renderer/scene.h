#pragma once
#include "renderer/primitives.h"
#include "renderer/material.h"
#include "renderer/skybox.h"
#include "renderer/texture.h"

// high level settings
// #define TWOLEVEL
#define WORLDSIZE	32 // power of 2. Warning: max 512 for a 512x512x512x4 bytes = 512MB world!
// #define USE_SIMD
// #define USE_FMA3
// #define SKYDOME
// #define WHITTED
// #define DOF

// low-level / derived
#define WORLDSIZE2	(WORLDSIZE*WORLDSIZE)
#ifdef TWOLEVEL
// feel free to replace with whatever suits your two-level implementation,
// should you chose this challenge.
#define BRICKSIZE	8
#define BRICKSIZE2	(BRICKSIZE*BRICKSIZE)
#define BRICKSIZE3	(BRICKSIZE*BRICKSIZE*BRICKSIZE)
#define GRIDSIZE	(WORLDSIZE/BRICKSIZE)
#define VOXELSIZE	(1.0f/WORLDSIZE)
#else
#define GRIDSIZE	WORLDSIZE
#endif
#define GRIDSIZE2	(GRIDSIZE*GRIDSIZE)
#define GRIDSIZE3	(GRIDSIZE*GRIDSIZE*GRIDSIZE)

inline uint8_t* createGrid()
{
	uint32_t bufferSize = 0;
	static const auto buffSizes = getGridBufferIndices<LOD_LEVELS, GRIDSIZE>();
	for (uint32_t i = 0; i < buffSizes.size(); i++)
	{
		bufferSize += buffSizes[i];
	}

	uint8_t* buffer = (uint8_t*)MALLOC64(bufferSize * sizeof(uint8_t));
	memset(buffer, 0, bufferSize * sizeof(uint8_t));
	return buffer;
}

struct Transform
{
	mat4 matrix;
	mat4 invMatrix;
	float3 position, rotation, scale;

	inline void ToMatrix()
	{
		matrix = createMatrix(position, rotation, scale);
		invMatrix = matrix.Inverted();
	}
};
	

class RenderLevel
{
public:
	void FindNearest( Ray& ray ) const;
	void FindExit(Ray& ray) const;
	bool IsOccluded( const Ray& ray ) const;

	void AddObject(Primitive& object, const float3& position, const float3& rotation, const float3& scale)
	{
		// Add the object
		if (object.type == PrimitiveType::Voxel)
			object.materialIndex = 0;

		object.matrixIndex = objectCount;

		objects[objectCount] = object;

		// Set the transform.
		transforms[objectCount].position = position;
		transforms[objectCount].rotation = rotation;
		transforms[objectCount].scale = scale;
		transforms[objectCount].ToMatrix();

		objectCount++;
	}

	void Reset()
	{
		objectCount = 0;

		pointLights.resize(0);
		directionalLights.resize(0);
		spotlights.resize(0);
		areaLights.resize(0);

		for (uint32_t i = 0; i < 64; i++)
		{
			materials[i] = Material();
		}
	}

	uint16_t GetObjectCount() const { return objectCount; }

public:
	Primitive objects[64];
	Material materials[64];
	std::vector<PointLight> pointLights{ 0 };
	std::vector<DirectionalLight> directionalLights{ 0 };
	std::vector<SpotLight> spotlights{ 0 };
	std::vector<AreaLight> areaLights{ 0 };
	Skybox skybox = Skybox("assets/sky.hdr");
	inline static Transform transforms[512];

private:
	uint16_t objectCount = 0;
};