#include "precomp.h"
#include "CppUnitTest.h"

inline Primitive getPrimitiveCube()
{
	// Create voxel volume test.
	Primitive voxelUnitTest;
	voxelUnitTest.grid = createGrid();
	for (uint32_t x = 0; x < GRIDSIZE; x++) for (uint32_t y = 0; y < GRIDSIZE; y++) for (uint32_t z = 0; z < GRIDSIZE; z++)
	{
		voxelUnitTest.SetVoxel(x, y, z, 1);
	}
	return voxelUnitTest;
}

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace RaygunCoreTests
{
	TEST_CLASS(RaygunCoreTests)
	{
	public:
			TEST_METHOD(VoxelIntersections1)
			{
				auto p = getPrimitiveCube();

				// This ray should hit.
				Ray r({ 0, 1.5f, 0 }, { 0, -1, 0 });
				Assert::IsTrue(p.HitVoxel(r));

				FREE64(p.grid);
			}

			TEST_METHOD(VoxelIntersections2)
			{
				auto p = getPrimitiveCube();

				// This ray should hit.
				Ray r({ 0, 1.5f, 0.9999f }, { 0, -1, 0 });
				Assert::IsTrue(p.HitVoxel(r));

				FREE64(p.grid);
			}

			TEST_METHOD(VoxelIntersections3)
			{
				auto p = getPrimitiveCube();

				// This ray should hit.
				Ray r({ 0, 1.5f, 0.5f }, { 0, -1, 0 });
				Assert::IsTrue(p.HitVoxel(r));

				FREE64(p.grid);
			}

			TEST_METHOD(VoxelIntersections4)
			{
				auto p = getPrimitiveCube();

				// This ray should not hit.
				Ray r({ -0.001f, 1.5f, 0 }, { 0, -1, 0 });
				Assert::IsFalse(p.HitVoxel(r));

				FREE64(p.grid);
			}

			TEST_METHOD(VoxelIntersections5)
			{
				auto p = getPrimitiveCube();

				// This ray should hit.
				Ray r({ 0, -0.5f, 0 }, { 0, 1, 0 });
				Assert::IsTrue(p.HitVoxel(r));

				FREE64(p.grid);
			}

			TEST_METHOD(VoxelIntersections6)
			{
				auto p = getPrimitiveCube();

				// This ray should hit.
				Ray r({ 0, -0.5f, 0.9999f }, { 0, 1, 0 });
				Assert::IsTrue(p.HitVoxel(r));

				FREE64(p.grid);
			}

			TEST_METHOD(VoxelIntersections7)
			{
				auto p = getPrimitiveCube();

				// This ray should hit.
				Ray r({ 0, -0.5f, 0.5f }, { 0, 1, 0 });
				Assert::IsTrue(p.HitVoxel(r));

				FREE64(p.grid);
			}

			TEST_METHOD(VoxelIntersections8)
			{
				auto p = getPrimitiveCube();

				// This ray should not hit.
				Ray r({ -0.001f, -0.5f, 0 }, { 0, 1, 0 });
				Assert::IsFalse(p.HitVoxel(r));

				FREE64(p.grid);
			}

			TEST_METHOD(SphereIntersections1)
			{
				// Create voxel volume test.
				Primitive sphereUnitTest;

				// This ray should hit.
				Ray r({ 0.49f, -0.5f, 0 }, { 0, 1, 0 });
				Assert::IsTrue(sphereUnitTest.HitSphere(r));
			}

			TEST_METHOD(SphereIntersections2)
			{
				// Create voxel volume test.
				Primitive sphereUnitTest;

				// This ray should not hit.
				Ray r2({ 0.51f, -0.5f, 0 }, { 0, 1, 0 });
				Assert::IsFalse(sphereUnitTest.HitSphere(r2));
			}

			TEST_METHOD(SphereIntersections3)
			{
				// Create voxel volume test.
				Primitive sphereUnitTest;

				// This ray should  hit.
				Ray r2({ 0.49f, 1.5f, 0 }, { 0, -1, 0 });
				Assert::IsTrue(sphereUnitTest.HitSphere(r2));
			}

			TEST_METHOD(SphereIntersections4)
			{
				// Create voxel volume test.
				Primitive sphereUnitTest;

				// This ray should not hit.
				Ray r2({ 0.51f, 1.5f, 0 }, { 0, -1, 0 });
				Assert::IsFalse(sphereUnitTest.HitSphere(r2));
			}
	};
}