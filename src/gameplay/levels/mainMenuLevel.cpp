#include "precomp.h"
#include "mainMenuLevel.h"
#include "gameplay/debugController.h"
#include "gameplay/gameManager.h"
#include "gameplay/entity.h"
#include "renderer/texture.h"

namespace Gameplay
{
	void MainMenuLevel::Init()
	{
		// Add debug screen entity.
		{
			DebugController* entt = new DebugController(this);
			entities.push_back(entt);
		}

		// Set material properties.
		{
			Material& material = Game::Get()->GetRenderer()->GetScene()->materials[0];
			material.type = MaterialType::LambertDiffuse;
			material.texture = new Texture("assets/wood.png");
		}

		{
			Material& material = Game::Get()->GetRenderer()->GetScene()->materials[1];
			material.type = MaterialType::LambertDiffuse;
			material.texture = new Texture("assets/checkerboard.png");
		}

		{
			Material& material = Game::Get()->GetRenderer()->GetScene()->materials[2];
			material.type = MaterialType::Mirror;
			material.specularity = 0.9f;
			material.color = { 1.0f, 0.8f, 1.0f };
		}

		{
			Material& material = Game::Get()->GetRenderer()->GetScene()->materials[3];
			material.type = MaterialType::Glossy;
			material.specularity = 0.9f;
			material.roughness = 0.5f;
			material.color = { 0.9f, 1.0f, 1.0f };
		}

		{
			Material& material = Game::Get()->GetRenderer()->GetScene()->materials[4];
			material.type = MaterialType::Glass;
			material.refractance = 1.005f;
			material.color = { 2.0f, 0.8f, 0.0f };
		}

		{
			Material& material = Game::Get()->GetRenderer()->GetScene()->materials[5];
			material.type = MaterialType::Glass;
			material.refractance = 2.4f;
		}

		{
			Material& material = Game::Get()->GetRenderer()->GetScene()->materials[6];
			material.type = MaterialType::Emissive;
			material.color = { 0.0f, 2.5f, 1.0f };
		}

		// Add spheres.
		for (uint32_t x = 0; x < 7; x++)
		{
			Primitive prim;
			prim.type = PrimitiveType::Sphere;
			prim.materialIndex = (uint16_t)x;
			Game::Get()->GetRenderer()->GetScene()->AddObject(prim, { x * 0.75f, 0.75f, 2.5f}, { 0, 0, 0 }, { 0.5f, 0.5f, 0.5f });

		}

		// Add voxel floor.
		{
			Primitive prim;
			prim.type = PrimitiveType::Voxel;
			prim.grid = createGrid();

			for (uint32_t x = 0; x < GRIDSIZE; x++) for (uint32_t z = 0; z < GRIDSIZE; z++)
			{
				const uint8_t mat = noise2D((float)x, (float)z) < 0.09f ? 0 : 1;
				prim.SetVoxel(x, 1, z, mat);
				prim.SetVoxel(x, 0, z, 1);
			}

			Game::Get()->GetRenderer()->GetScene()->AddObject(prim, { 0, 0, 0 }, { 0, 0, 0 }, { 8, 4, 8 });
		}

		Game::Get()->GetRenderer()->GetCamera()->camPos = { 2.5f, 0.75f, 5.0f };
		Game::Get()->GetRenderer()->GetCamera()->camTarget = Game::Get()->GetRenderer()->GetCamera()->camPos + float3{ 0.0f, 0.0f, -1.0f };

		Game::Get()->GetRenderer()->GetScene()->skybox.intensity = 0.25f;

		{
			PointLight light;
			light.color = { 0.0f, 0.0f, 1.0f };
			light.position = { 1.0f, 1.0f, 1.0f };
			light.radius = 1.0f;
			Game::Get()->GetRenderer()->GetScene()->pointLights.push_back(light);
		}

		{
			PointLight light;
			light.color = { 1.0f, 0.0f, 0.0f};
			light.position = { 3.0f, 1.0f, 1.0f };
			light.radius = 1.0f;
			Game::Get()->GetRenderer()->GetScene()->pointLights.push_back(light);
		}
		{
			PointLight light;
			light.color = { 0.0f, 1.0f, 0.0f };
			light.position = { 5.0f, 1.0f, 1.0f };
			light.radius = 1.0f;
			Game::Get()->GetRenderer()->GetScene()->pointLights.push_back(light);
		}

		{
			SpotLight light;
			light.color = { 0.9f, 0.7f, 0.0f };
			light.position = { 3.0f, 1.5f, 4.0f };
			light.radius = 1.0f;
			light.direction = normalize({ -0.25f, -0.25f, 0.0f });
			light.outerCutoff = 0.9f;
			light.cutoff = 0.8f;
			Game::Get()->GetRenderer()->GetScene()->spotlights.push_back(light);
		}

		{
			DirectionalLight light;
			light.color = float3{ 1.0f, 0.965f, 0.878f } * 0.25f;
			light.direction = normalize({ 0.2f, -1.0f, 0.3f });
			Game::Get()->GetRenderer()->GetScene()->directionalLights.push_back(light);
		}

		{
			AreaLight light;
			light.color = { 0.2f, 0.0f, 0.5f };
			light.width = 1.0f;
			light.height = 1.0f;
			light.position = { 4.0f, 0.5f, 4.0f };
			Game::Get()->GetRenderer()->GetScene()->areaLights.push_back(light);
		}

		for (Entity*& entt : entities)
		{
			entt->Init();
		}
	}
}