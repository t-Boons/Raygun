#include "precomp.h"

// From: https://raytracing.github.io/books/RayTracingInOneWeekend.html
void Primitive::HitEntrySphere(const Ray& r, float& tOut) const
{
	const float3 oc = r.O;
	const float a = dot(r.D, r.D);
	const float b = 2.0f * dot(oc, r.D);
	const float c = dot(oc, oc) - 0.25f;
	const float discriminant = b * b - 4 * a * c;

	if (discriminant < 0)
	{
		tOut = -1.0f;
	}
	else
	{
		tOut = (-b - sqrtf(discriminant)) / (2.0f * a);
	}
}

// Created by Jacco Bikker
bool Primitive::HitSphere(const Ray& r) const
{
	const float3 oc = r.O;
	const float a = dot(r.D, r.D);
	const float b = 2.0f * dot(oc, r.D);
	const float c = dot(oc, oc) - 0.25f;
	const float discriminant = b * b - 4 * a * c;

	if (discriminant < 0)
	{
		return false;
	}
	else
	{
		return (-b - sqrtf(discriminant)) / (2.0f * a) > 0.001f;
	}
}

// From: https://raytracing.github.io/books/RayTracingInOneWeekend.html
void Primitive::HitExitSphere(const Ray& r, float& t) const
{
	const float3 oc = r.O;
	const float a = dot(r.D, r.D);
	const float b = 2.0f * dot(oc, r.D);
	const float c = dot(oc, oc) - 0.25f;
	const float discriminant = b * b - 4 * a * c;

	if (discriminant < 0)
	{
		t = -1.0f;
	}
	else
	{
		t = (-b + sqrtf(discriminant)) / (2.0f * a);
	}
}

// Created by Jacco Bikker
Cube::Cube(const float3 pos, const float3 size)
{
	// set cube bounds
	b[0] = pos;
	b[1] = pos + size;
}

// Returns t (distance from intersection) // Created by Jacco Bikker
float Cube::Intersect(const Ray& ray) const
{
	// test if the ray intersects the cube


	const int signx = ray.D.x < 0, signy = ray.D.y < 0, signz = ray.D.z < 0;

	// 0 or 1 based on if the ray direction is negative.
	// tmin x axis. used later.
	float tmin = (b[signx].x - ray.O.x) * ray.rD.x;
	float tmax = (b[1 - signx].x - ray.O.x) * ray.rD.x;


	const float tymin = (b[signy].y - ray.O.y) * ray.rD.y;
	const float tymax = (b[1 - signy].y - ray.O.y) * ray.rD.y;

	if (tmin > tymax || tymin > tmax) goto miss;
	tmin = max(tmin, tymin), tmax = min(tmax, tymax);

	const float tzmin = (b[signz].z - ray.O.z) * ray.rD.z;
	const float tzmax = (b[1 - signz].z - ray.O.z) * ray.rD.z;

	if (tmin > tzmax || tzmin > tmax) goto miss; // yeah c has 'goto' ;)
	if ((tmin = max(tmin, tzmin)) > 0) return tmin;
miss:
	return 1e34f;
}

// Created by Jacco Bikker
bool Cube::Contains(const float3& pos) const
{
	// test if pos is inside the cube // AABB type calculation.
	return pos.x >= b[0].x && pos.y >= b[0].y && pos.z >= b[0].z &&
		pos.x <= b[1].x && pos.y <= b[1].y && pos.z <= b[1].z;
}

void Primitive::HitEntryVoxel(const Ray& r, float& t, uint32_t& voxelIndex) const
{
#if USE_MULTILEVEL_TRAVERSAL
	t = -1.0f;

	DDAState s;
	s.t = 0;
	if(!contains({0, 0, 0}, {1, 1, 1}, r.O))
	{
		s.t = intersects({ 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, r);
		if (s.t > 1e33f) return; // ray misses voxel data entirely
	}

	s.step = make_int3(1 - r.Dsign * 2);

	int lod = LOD_LEVELS - 1;

	while (lod >= 0)
	{
		static const std::array<int, LOD_LEVELS> gridSizes = getGridSizes<LOD_LEVELS, GRIDSIZE>();
		static const std::array<int, LOD_LEVELS> gridSizes2 = getGridSizes2<LOD_LEVELS, GRIDSIZE>();
		static const std::array<float, LOD_LEVELS> cellSizes = getGridCellSizes<LOD_LEVELS, GRIDSIZE>();
		static const std::array<int, LOD_LEVELS> bufferIndex = getGridBufferIndices<LOD_LEVELS, GRIDSIZE>();

		const uint32_t gridSize = gridSizes[lod];
		const uint32_t gridSize2 = gridSizes2[lod];

		const float cellSize = cellSizes[lod];
		const float3 posInGrid = (float)gridSize * (r.O + (s.t + 0.00005f) * r.D);
		const float3 gridPlanes = (ceilf(posInGrid) - r.Dsign) * cellSize;
		const int3 P = clamp(make_int3(posInGrid), 0, gridSize - 1);
		s.X = P.x, s.Y = P.y, s.Z = P.z;
		s.tdelta = cellSize * float3(s.step) * r.rD;

		s.tmax = (gridPlanes - r.O) * r.rD;

		const uint8_t* gridStart = &grid[bufferIndex[lod]];

		// start stepping
		while (1)
		{
			const uint8_t cell = gridStart[s.X + s.Y * gridSize + s.Z * gridSize2];
			if (cell)
			{
				if (lod == 0)
				{
					t = s.t;
					voxelIndex = s.X + s.Y * gridSize + s.Z * gridSize2;
				}

				lod--;
				break;
			}
			if (s.tmax.x < s.tmax.y)
			{
				if (s.tmax.x < s.tmax.z) { s.t = s.tmax.x, s.X += s.step.x; if (s.X >= gridSize) return; s.tmax.x += s.tdelta.x; }
				else { s.t = s.tmax.z, s.Z += s.step.z; if (s.Z >= gridSize) return; s.tmax.z += s.tdelta.z; }
			}
			else
			{
				if (s.tmax.y < s.tmax.z) { s.t = s.tmax.y, s.Y += s.step.y; if (s.Y >= gridSize) return; s.tmax.y += s.tdelta.y; }
				else { s.t = s.tmax.z, s.Z += s.step.z; if (s.Z >= gridSize) return; s.tmax.z += s.tdelta.z; }
			}
		}
	}
#else
	t = -1.0f;

	// Created by Jacco Bikker
	DDAState s;
	if (!Setup3DDDA(r, s)) return;
	// start stepping
	while (1)
	{
		const uint32_t cell = grid[s.X + s.Y * GRIDSIZE + s.Z * GRIDSIZE2];
		if (cell)
		{
			t = s.t;
			voxelIndex = s.X + s.Y * GRIDSIZE + s.Z * GRIDSIZE2;
			break;
		}
		if (s.tmax.x < s.tmax.y)
		{
			if (s.tmax.x < s.tmax.z) { s.t = s.tmax.x, s.X += s.step.x; if (s.X >= GRIDSIZE) break; s.tmax.x += s.tdelta.x; }
			else { s.t = s.tmax.z, s.Z += s.step.z; if (s.Z >= GRIDSIZE) break; s.tmax.z += s.tdelta.z; }
		}
		else
		{
			if (s.tmax.y < s.tmax.z) { s.t = s.tmax.y, s.Y += s.step.y; if (s.Y >= GRIDSIZE) break; s.tmax.y += s.tdelta.y; }
			else { s.t = s.tmax.z, s.Z += s.step.z; if (s.Z >= GRIDSIZE) break; s.tmax.z += s.tdelta.z; }
		}
	}
#endif
}


void Primitive::HitExitVoxel(const Ray& r, float& t, uint32_t& voxelIndex) const
{
#if USE_MULTILEVEL_TRAVERSAL
	t = 1.0f;

	DDAState s;
	s.t = 0;
	if (!contains({ 0, 0, 0 }, { 1, 1, 1 }, r.O))
	{
		s.t = intersects({ 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, r);
		if (s.t > 1e33f) return; // ray misses voxel data entirely
	}

	s.step = make_int3(1 - r.Dsign * 2);

	int lod = LOD_LEVELS - 1;

	while (lod >= 0)
	{
		static const std::array<int, LOD_LEVELS> gridSizes = getGridSizes<LOD_LEVELS, GRIDSIZE>();
		static const std::array<int, LOD_LEVELS> gridSizes2 = getGridSizes2<LOD_LEVELS, GRIDSIZE>();
		static const std::array<float, LOD_LEVELS> cellSizes = getGridCellSizes<LOD_LEVELS, GRIDSIZE>();
		static const std::array<int, LOD_LEVELS> bufferIndex = getGridBufferIndices<LOD_LEVELS, GRIDSIZE>();

		const uint32_t gridSize = gridSizes[lod];
		const uint32_t gridSize2 = gridSizes2[lod];

		const float cellSize = cellSizes[lod];
		const float3 posInGrid = (float)gridSize * (r.O + (s.t + 0.00005f) * r.D);
		const float3 gridPlanes = (ceilf(posInGrid) - r.Dsign) * cellSize;
		const int3 P = clamp(make_int3(posInGrid), 0, gridSize - 1);
		s.X = P.x, s.Y = P.y, s.Z = P.z;
		s.tdelta = cellSize * float3(s.step) * r.rD;

		s.tmax = (gridPlanes - r.O) * r.rD;


		const uint8_t* gridStart = &grid[bufferIndex[lod]];

		// start stepping
		while (1)
		{
			const uint8_t cell = gridStart[s.X + s.Y * gridSize + s.Z * gridSize2];
			if (!cell)
			{
				if (lod == 0)
				{
					t = s.t;
					voxelIndex = s.X + s.Y * gridSize + s.Z * gridSize2;
				}

				lod--;
				break;
			}
			if (s.tmax.x < s.tmax.y)
			{
				if (s.tmax.x < s.tmax.z) { s.t = s.tmax.x, s.X += s.step.x; if (s.X >= gridSize) return; s.tmax.x += s.tdelta.x; }
				else { s.t = s.tmax.z, s.Z += s.step.z; if (s.Z >= gridSize) return; s.tmax.z += s.tdelta.z; }
			}
			else
			{
				if (s.tmax.y < s.tmax.z) { s.t = s.tmax.y, s.Y += s.step.y; if (s.Y >= gridSize) return; s.tmax.y += s.tdelta.y; }
				else { s.t = s.tmax.z, s.Z += s.step.z; if (s.Z >= gridSize) return; s.tmax.z += s.tdelta.z; }
			}
		}
	}
#else
	t = 1.0f;

	// Created by Jacco Bikker
	DDAState s;
	if (!Setup3DDDA(r, s)) return;
	// start stepping
	while (1)
	{
		const uint32_t cell = grid[s.X + s.Y * GRIDSIZE + s.Z * GRIDSIZE2];
		if (!cell)
		{
			t = s.t;
			voxelIndex = s.X + s.Y * GRIDSIZE + s.Z * GRIDSIZE2;
			break;
		}
		if (s.tmax.x < s.tmax.y)
		{
			if (s.tmax.x < s.tmax.z) { s.t = s.tmax.x, s.X += s.step.x; if (s.X >= GRIDSIZE) break; s.tmax.x += s.tdelta.x; }
			else { s.t = s.tmax.z, s.Z += s.step.z; if (s.Z >= GRIDSIZE) break; s.tmax.z += s.tdelta.z; }
		}
		else
		{
			if (s.tmax.y < s.tmax.z) { s.t = s.tmax.y, s.Y += s.step.y; if (s.Y >= GRIDSIZE) break; s.tmax.y += s.tdelta.y; }
			else { s.t = s.tmax.z, s.Z += s.step.z; if (s.Z >= GRIDSIZE) break; s.tmax.z += s.tdelta.z; }
		}
	}
#endif
}

// Created by Jacco Bikker
bool Primitive::HitVoxel(const Ray& r) const
{
#if USE_MULTILEVEL_TRAVERSAL
	DDAState s;
	s.t = 0;
	if (!contains({ 0, 0, 0 }, { 1, 1, 1 }, r.O))
	{
		s.t = intersects({ 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, r);
		if (s.t > 1e33f) return false; // ray misses voxel data entirely
	}

	s.step = make_int3(1 - r.Dsign * 2);

	int lod = LOD_LEVELS - 1;

	while (lod >= 0)
	{
		static const std::array<int, LOD_LEVELS> gridSizes = getGridSizes<LOD_LEVELS, GRIDSIZE>();
		static const std::array<int, LOD_LEVELS> gridSizes2 = getGridSizes2<LOD_LEVELS, GRIDSIZE>();
		static const std::array<float, LOD_LEVELS> cellSizes = getGridCellSizes<LOD_LEVELS, GRIDSIZE>();
		static const std::array<int, LOD_LEVELS> bufferIndex = getGridBufferIndices<LOD_LEVELS, GRIDSIZE>();

		const uint32_t gridSize = gridSizes[lod];
		const uint32_t gridSize2 = gridSizes2[lod];

		const float cellSize = cellSizes[lod];

		const float3 posInGrid = (float)gridSize * (r.O + (s.t + 0.00005f) * r.D);
		const float3 gridPlanes = (ceilf(posInGrid) - r.Dsign) * cellSize;
		const int3 P = clamp(make_int3(posInGrid), 0, gridSize - 1);
		s.X = P.x, s.Y = P.y, s.Z = P.z;
		s.tdelta = cellSize * float3(s.step) * r.rD;

		s.tmax = (gridPlanes - r.O) * r.rD;


		const uint8_t* gridStart = &grid[bufferIndex[lod]];

		// start stepping
		while (1)
		{
			const uint8_t cell = gridStart[s.X + s.Y * gridSize + s.Z * gridSize2];
			if (cell)
			{ 
				if (lod == 0)
				{
					return s.t < r.t;
				}
				lod--;
				break;
			}
			if (s.tmax.x < s.tmax.y)
			{
				if (s.tmax.x < s.tmax.z) { s.t = s.tmax.x, s.X += s.step.x; if (s.X >= gridSize) return false; s.tmax.x += s.tdelta.x; }
				else { s.t = s.tmax.z, s.Z += s.step.z; if (s.Z >= gridSize) return false; s.tmax.z += s.tdelta.z; }
			}
			else
			{
				if (s.tmax.y < s.tmax.z) { s.t = s.tmax.y, s.Y += s.step.y; if (s.Y >= gridSize) return false; s.tmax.y += s.tdelta.y; }
				else { s.t = s.tmax.z, s.Z += s.step.z; if (s.Z >= gridSize) return false; s.tmax.z += s.tdelta.z; }
			}
		}
	}

	return false;
#else
	// setup Amanatides & Woo grid traversal
	DDAState s;
	if (!Setup3DDDA(r, s)) return false;
	// start stepping
	while (s.t < r.t)
	{
		const uint32_t cell = grid[s.X + s.Y * GRIDSIZE + s.Z * GRIDSIZE2];
		if (cell)
		{
			return s.t < r.t && s.t > 0.0f;
		}
		if (s.tmax.x < s.tmax.y)
		{
			if (s.tmax.x < s.tmax.z) { if ((s.X += s.step.x) >= GRIDSIZE) return false; s.t = s.tmax.x, s.tmax.x += s.tdelta.x; }
			else { if ((s.Z += s.step.z) >= GRIDSIZE) return false; s.t = s.tmax.z, s.tmax.z += s.tdelta.z; }
		}
		else
		{
			if (s.tmax.y < s.tmax.z) { if ((s.Y += s.step.y) >= GRIDSIZE) return false; s.t = s.tmax.y, s.tmax.y += s.tdelta.y; }
			else { if ((s.Z += s.step.z) >= GRIDSIZE) return false; s.t = s.tmax.z, s.tmax.z += s.tdelta.z; }
		}
	}
	return true;
#endif
}

// Created by Jacco Bikker
bool Primitive::Setup3DDDA(const Ray& ray, DDAState& state) const
{
	// if ray is not inside the world: advance until it is
	state.t = 0;
	if (!contains({0, 0, 0}, { 1, 1, 1 }, ray.O))
	{
		state.t = intersects({ 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, ray);
		if (state.t > 1e33f) return false; // ray misses voxel data entirely
	}
	// setup amanatides & woo - assume world is 1x1x1, from (0,0,0) to (1,1,1)
	const float cellSize = 1.0f / GRIDSIZE;
	state.step = make_int3(1 - ray.Dsign * 2);
	const float3 posInGrid = GRIDSIZE * (ray.O + (state.t + 0.00005f) * ray.D);
	const float3 gridPlanes = (ceilf(posInGrid) - ray.Dsign) * cellSize;
	const int3 P = clamp(make_int3(posInGrid), 0, GRIDSIZE - 1);
	state.X = P.x, state.Y = P.y, state.Z = P.z;
	state.tdelta = cellSize * float3(state.step) * ray.rD;

	state.tmax = (gridPlanes - ray.O) * ray.rD;

	// proceed with traversal
	return true;
}

void Primitive::SetVoxel(uint32_t x, uint32_t y, uint32_t z, uint8_t mat)
{
	grid[x + y * GRIDSIZE + z * GRIDSIZE2] = mat;

	static const std::array<int, LOD_LEVELS> gridSizes = getGridSizes<LOD_LEVELS, GRIDSIZE>();
	static const std::array<int, LOD_LEVELS> gridSizes2 = getGridSizes2<LOD_LEVELS, GRIDSIZE>();
	static const std::array<int, LOD_LEVELS> bufferIndex = getGridBufferIndices<LOD_LEVELS, GRIDSIZE>();

	int lod = 0;

	while (lod < LOD_LEVELS - 1)
	{
		uint8_t* gridStart = &grid[bufferIndex[lod]];

		uint8_t v = 0;
		v = gridStart[x + y * gridSizes[lod] + z * gridSizes2[lod]];
		if (v > 0) goto voxelFound;
		v = gridStart[(x + 1) + y * gridSizes[lod] + z * gridSizes2[lod]];
		if (v > 0) goto voxelFound;
		v = gridStart[(x + 1) + (y + 1) * gridSizes[lod] + z * gridSizes2[lod]];
		if (v > 0) goto voxelFound;
		v = gridStart[x + (y + 1) * gridSizes[lod] + z * gridSizes2[lod]];
		if (v > 0) goto voxelFound;
		v = gridStart[x + y * gridSizes[lod] + (z + 1) * gridSizes2[lod]];
		if (v > 0) goto voxelFound;
		v = gridStart[(x + 1) + y * gridSizes[lod] + (z + 1) * gridSizes2[lod]];
		if (v > 0) goto voxelFound;
		v = gridStart[(x + 1) + (y + 1) * gridSizes[lod] + (z + 1) * gridSizes2[lod]];
		if (v > 0) goto voxelFound;
		v = gridStart[x + (y + 1) * gridSizes[lod] + (z + 1) * gridSizes2[lod]];
		if (v > 0) goto voxelFound;
	voxelFound:

		if(x > 0) x /= 2;
		if (y > 0) y /= 2;
		if (z > 0) z /= 2;

		grid[bufferIndex[lod + 1] + x + y * gridSizes[lod + 1] + z * gridSizes2[lod + 1]] = v;

		lod++;
	}
}
