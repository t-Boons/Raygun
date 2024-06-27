#pragma once
#include "precomp.h"

#define USE_MULTILEVEL_TRAVERSAL 1
#define LOD_LEVELS 3

class Ray;

class Cube
{
public:
	Cube() = default;
	Cube(const float3 pos, const float3 size);
	float Intersect(const Ray& ray) const;
	bool Contains(const float3& pos) const;
	float3 b[2];
};

class ALIGN(16) Primitive
{
public:
	struct DDAState
	{
		int3 step;				// 16 bytes
		uint32_t X, Y, Z;			// 12 bytes
		float t;				// 4 bytes
		float3 tdelta;
		float dummy1 = 0;		// 16 bytes
		float3 tmax;
		float dummy2 = 0;		// 16 bytes, 64 bytes in total
	};


	void HitEntry(const Ray& r, float& t, uint32_t& voxelObject) const
	{
		switch (type)
		{
		case PrimitiveType::Voxel: HitEntryVoxel(r, t, voxelObject); return;
		case PrimitiveType::Sphere: HitEntrySphere(r, t); return;
		}
	}

	void HitExit(const Ray& r, float& t, uint32_t& voxelObject) const
	{
		switch (type)
		{
		case PrimitiveType::Voxel: HitExitVoxel(r, t, voxelObject); return;
		case PrimitiveType::Sphere: HitExitSphere(r, t); return;
		}
	}

	bool Hit(const Ray& r) const
	{
		switch (type)
		{
		case PrimitiveType::Voxel:  return HitVoxel(r);
		case PrimitiveType::Sphere: return HitSphere(r);
		}

		return false;
	}


	void HitEntrySphere(const Ray& r, float& t) const;
	void HitExitSphere(const Ray& r, float& t) const;
	bool HitSphere(const Ray& r) const;

	void HitEntryVoxel(const Ray & r, float& t, uint32_t& voxelIndex) const;
	void HitExitVoxel(const Ray& r, float& t, uint32_t& voxelIndex) const;
	bool HitVoxel(const Ray& r) const;

	// Created by Jacco Bikker
	bool Setup3DDDA(const Ray& ray, DDAState& state) const;


	void SetVoxel(uint32_t x, uint32_t y, uint32_t z, uint8_t mat);

	PrimitiveType type = PrimitiveType::None; // 1
	uint16_t materialIndex = 0; // 2
	uint16_t matrixIndex = 0;   // 2
	uint8_t* grid; // 8						// 13
};