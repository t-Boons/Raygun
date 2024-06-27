#include "precomp.h"

void RenderLevel::FindNearest(Ray& ray) const
{
	float tClosest = 1e34f;
	float t; uint32_t voxelObject;

	for (uint32_t i = 0; i < objectCount; i++)
	{
		const mat4& objMatrix = RenderLevel::transforms[objects[i].matrixIndex].invMatrix;
		const Ray copiedRay = Ray(TransformPosition(ray.O, objMatrix),
						      TransformVector(ray.D, objMatrix));



		 objects[i].HitEntry(copiedRay, t, voxelObject);

		if (t > 0)
		{
			if (t < tClosest)
			{
				tClosest = t;
				ray.t = t;
				ray.object = &objects[i];
				ray.voxelIndex = voxelObject;
			}
		}
	}
}

void RenderLevel::FindExit(Ray& ray) const
{
	float tClosest = 1e34f;

	for (uint32_t i = 0; i < objectCount; i++)
	{
		float t; uint32_t voxelObject;

		const mat4& objMatrix = RenderLevel::transforms[objects[i].matrixIndex].invMatrix;
		const Ray copiedRay = Ray(TransformPosition(ray.O, objMatrix),
			TransformVector(ray.D, objMatrix));

		objects[i].HitExit(copiedRay, t, voxelObject);

		if (t > 0)
		{
			if (t < tClosest)
			{
				tClosest = t;
				ray.t = t;
				ray.object = &objects[i];
				ray.voxelIndex = voxelObject;
			}
		}
	}
}

bool RenderLevel::IsOccluded(const Ray& ray) const
{
	// Loop through all objects.
	for (uint32_t i = 0; i < objectCount; i++)
	{
		const mat4& objMatrix = RenderLevel::transforms[objects[i].matrixIndex].invMatrix;
		const Ray copiedRay = Ray(TransformPosition(ray.O, objMatrix),
			TransformVector(ray.D, objMatrix));

		

		if (objects[i].Hit(copiedRay))
		{
			return true;
		}
	}

	return false;
}