#include "precomp.h"

float intersects(const float3& bl, const float3& tr, const Ray& ray)
{
	// test if the ray intersects the cube
	const float3 b[2] = { bl, tr };

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

	if (tmin > tzmax || tzmin > tmax) goto miss;
	if ((tmin = max(tmin, tzmin)) > 0) return tmin;	
miss:
	return 1e34f;
}

float2 getUV(const float3& I, const float3& N, PrimitiveType type)
{
	switch (type)
	{
		case PrimitiveType::Voxel:
		{
			// Code by: Remi (Peer)
			float u = 0.0f, v = 0.0f;
			float x = fracf(I.x * WORLDSIZE);
			float y = fracf(I.y * WORLDSIZE);
			float z = fracf(I.z * WORLDSIZE);

			if (N.x != 0 || N.z != 0) v = y;
			if (N.x != 0) u = z;
			else if (N.z != 0) u = x;
			if (N.y != 0) { u = x; v = z; }
			return float2(u, v);
		}

		case PrimitiveType::Sphere:
		{
			const float3 ndir = normalize(N);

			// Calculate uv coordinate in the texture space.
			const float theta = acos(ndir.y);
			const float phi = atan2(ndir.x, ndir.z);

			const float u = (phi / TWOPI + 0.5f);
			const float v = (1.0f - theta * INVPI);

			return float2(u, v);
		}
	}

	return float2(0);
}

float3 getNormal(const float3& I, const float3& Dsign, PrimitiveType type)
{
	switch (type)
	{
		case PrimitiveType::Voxel: 
		{
			const float3 I1 = (I)*WORLDSIZE; // our scene size is (1,1,1), so this scales each voxel to (1,1,1)
			const float3 fG = fracf(I1);
			const float3 d = min3(fG, 1.0f - fG);
			const float mind = min(min(d.x, d.y), d.z);
			const float3 sign = Dsign * 2 - 1;

			const float3 n = float3(mind == d.x ? sign.x : 0, mind == d.y ? sign.y : 0, mind == d.z ? sign.z : 0);
			return normalize(n);
		}

		case PrimitiveType::Sphere: 
		{
			return normalize(I / 1.0f);
		}
	}

	return float3(0);
}
