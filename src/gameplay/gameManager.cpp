#include "precomp.h"
#include "renderer/texture.h"
#include "gameManager.h"
#include "lightManipulator.h" 

namespace Gameplay
{
	void GameManager::Init()
	{
		Settings::stochasticLights = false;

		// Set material properties.
		{
			renderLevel->materials[1].type = MaterialType::LambertDiffuse;
			renderLevel->materials[1].color = float3(1.0f, 1.0f, 1.0f);
			renderLevel->materials[1].texture = new Texture("assets/checkerboard_light.png");
			renderLevel->materials[2].type = MaterialType::Mirror;
			renderLevel->materials[2].color = float3(1.5f);
			renderLevel->materials[2].specularity = 1.0f;
			renderLevel->materials[3].type = MaterialType::Glossy;
			renderLevel->materials[3].color = float3(1.0f);
			renderLevel->materials[3].specularity = 0.65f;
			renderLevel->materials[3].roughness = 0.02f;
			renderLevel->materials[4].type = MaterialType::Glass;
			renderLevel->materials[4].color = float3(0.0f);
			renderLevel->materials[4].refractance = 2.3f;
		}

		{
			// Add the middle sphere.
			Primitive primitive;
			primitive.type = PrimitiveType::Sphere;
			primitive.materialIndex = 2;

			renderLevel->AddObject(primitive, { 0.5f, 0.5f, 0.5f }, { 0, 0, 0 }, { 0.2f, 0.2f , 0.2f });
		}


		{
			// Add the voxel volume that can be manipulated.
			Primitive primitive;
			primitive.type = PrimitiveType::Voxel;
			primitive.grid = createGrid();
			primitive.materialIndex = 1;

			// Set the floor level.
			for (uint32_t x = 0; x < GRIDSIZE; x++) for (uint32_t y = 0; y < GRIDSIZE; y++)	for (uint32_t z = 0; z < GRIDSIZE; z++)
			{
				primitive.SetVoxel(x, 0, y, 1);
			}

			// Set the sides.
			for (uint32_t x = 0; x < GRIDSIZE; x++) for (uint32_t y = 0; y < GRIDSIZE / 2; y++)	for (uint32_t z = 0; z < GRIDSIZE; z++)
			{
				primitive.SetVoxel(x, y, GRIDSIZE - 1, 4);
				primitive.SetVoxel(GRIDSIZE - 1, y, x, 4);
			}

			AddToRenderlevel(primitive);
		}

		{
			// Add the point lights.
			std::vector<PointLight>& lights = renderLevel->pointLights;
			lights.resize(3);
			lights[0].color = { 1, 0, 0 };
			lights[1].color = { 0, 1, 0 };
			lights[2].color = { 0, 0, 1 };
		}

		renderLevel->skybox.intensity = 0.5f;
		Game::Get()->GetRenderer()->GetCamera()->camPos = float3{-5, 6, -5} *0.8f;
		Game::Get()->GetRenderer()->GetCamera()->camTarget = normalize({ 1, -0.25f, 1 });
		Settings::fov = 84;

		ResetLevel();
	}

	void GameManager::Tick(float deltaTime)
	{
		Entity::Tick(deltaTime);

		const float3 rPos = float3((float)randomPoint.x / (float)GRIDSIZE, 0.1f, (float)randomPoint.y / (float)GRIDSIZE);
		Game::Get()->GetRenderer()->DrawLine(rPos, rPos - float3{ 0, 0.1f, 0 }, ColorToColor32(randomColor));
		Game::Get()->GetRenderer()->DrawLine(rPos, rPos - float3{ -0.01f, 0.1f, 0.01f }, ColorToColor32(randomColor));
		Game::Get()->GetRenderer()->DrawLine(rPos, rPos - float3{ -0.01f, 0.1f, -0.01f }, ColorToColor32(randomColor));

		if (Game::Get()->GetRenderer()->NumAccumulatedFrames() < 12) tickTime = 0;

		tickTime += deltaTime * 0.001f;
		if (tickTime > 1)
		{
			isOn = !isOn;
			tickTime = 0;

			SceneData data{ random.RandomUInt()};
			Ray r = Ray(rPos, { 0, -1, 0 });
			const float3 pointColor = Game::Get()->GetRenderer()->Trace(r, data);

			switch (randomColor)
			{
			case Color::Red:
			{
				if (pointColor.x > pointColor.y + 0.8f && pointColor.x > pointColor.z + 0.8f && pointColor.x > 1.0f)
				{
					ResetLevel();
				}
			}
			break;
			case Color::Green:
			{
				if (pointColor.y > pointColor.x + 0.8f && pointColor.y > pointColor.z + 0.8f && pointColor.y > 1.0f)
				{
					ResetLevel();
				}
			}
			break;
			case Color::Blue:
			{
				if (pointColor.z > pointColor.y + 0.8f && pointColor.z > pointColor.x + 0.8f && pointColor.z > 1.0f)
				{
					ResetLevel();
				}
			}
			break;
			}

		}
	}

	void GameManager::ResetLevel()
	{
		Game::Get()->GetRenderer()->GetScene()->skybox.rotation += random.Float() * TWOPI;
		level->GetEntity<LightManipulator>()->ResetLights();
		randomPoint = { random.RandomUInt() % (GRIDSIZE - 1) + 1, random.RandomUInt() % (GRIDSIZE - 1) + 1 };
		randomColor = (Color)(random.RandomUInt() % 3);

		// Create an effect to make all the voxels appear and dissapear.
		std::thread t([=]()
			{
				Random rand((uint32_t)time(NULL));
				for (uint32_t x = 0; x < GRIDSIZE; x++)
				{
					Sleep(1);
					for (uint32_t y = 0; y < GRIDSIZE; y++)
					{
						for (uint32_t z = 0; z < GRIDSIZE; z++)
						{
							renderable->SetVoxel(y, x, z, 1);
						}
					}
				}

				for (uint32_t x = GRIDSIZE -1 ; x > 0; x--)
				{
					Sleep(1);
					for (uint32_t y = 0; y < GRIDSIZE; y++)
					{
						for (uint32_t z = 0; z < GRIDSIZE; z++)
						{
							renderable->SetVoxel(y, x, z, 0);
						}
					}
				}

				// Set the floor level.
				for (uint32_t x = 0; x < GRIDSIZE; x++) for (uint32_t y = 0; y < GRIDSIZE; y++)	for (uint32_t z = 0; z < GRIDSIZE; z++)
				{
					renderable->SetVoxel(x, 0, y, 1);
				}

				// Set the sides.
				for (uint32_t x = 0; x < GRIDSIZE; x++) for (uint32_t y = 0; y < GRIDSIZE / 2; y++)	for (uint32_t z = 0; z < GRIDSIZE; z++)
				{
					renderable->SetVoxel(x, y, GRIDSIZE - 1, 4);
					renderable->SetVoxel(GRIDSIZE - 1, y, x, 4);
				}

				Game::Get()->GetRenderer()->ResetAccumulator();
			});

		t.detach();
	}
}