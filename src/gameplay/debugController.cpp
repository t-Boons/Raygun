#include "precomp.h"
#include "debugController.h"
#include "renderer/scene.h"
#include "renderer/texture.h"
#include "gameplay/levels/gameplayLevel.h"

namespace Gameplay
{
	void DebugController::Init()
	{
	}

	void DebugController::TickUI()
	{
		ImGui::SetNextWindowPos(ImVec2(0.0f, ImGui::GetIO().DisplaySize.y - ImGui::GetIO().DisplaySize.y * 0.2f));
		ImGui::SetNextWindowSize({ ImGui::GetIO().DisplaySize.x , ImGui::GetIO().DisplaySize.y * 0.2f });

		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
		ImGui::Begin("Debug Info", false, windowFlags);
			

		// Print info.
		ImGui::Text("-- Use WASD and the arrow keys to move around.");
		ImGui::Text("-- Point at an object and press R to select it to get object properties.");
		ImGui::Text("-- You can scroll in this window.");

		// Play the game.
		if (ImGui::Button("Play the game!"))
		{
			Game::Get()->LoadLevel(new GameplayLevel());
			ImGui::End();
		}

		Renderer* renderer = Game::Get()->GetRenderer();

		if (selectedPrimitive)
		{
			Transform& transform = renderLevel->transforms[selectedPrimitive->matrixIndex];

			ImGui::Text(std::to_string(selectedPrimitive->matrixIndex).c_str());

			if (ImGui::DragFloat3("position", &transform.position[0], 0.05f))
				renderer->ResetAccumulator();
			if (ImGui::DragFloat3("Rotation", &transform.rotation[0], 0.05f))
				renderer->ResetAccumulator();
			if (ImGui::DragFloat3("Scale", &transform.scale[0], 0.05f, 0.0f, INFINITY))
				renderer->ResetAccumulator();

			renderLevel->transforms[selectedPrimitive->matrixIndex].ToMatrix();


			const int matIndex = selectedPrimitive->type == PrimitiveType::Voxel ?
				selectedPrimitive->grid[selectedVoxel] : selectedPrimitive->materialIndex;
			Material& mat = renderLevel->materials[matIndex];


			ImGui::Spacing();

			if (ImGui::InputText("Mat Texture", &inputText[0], 512))
			{
				if (fileExists(inputText))
				{
					delete mat.texture;
					mat.texture = new Texture(inputText);
				}
				else
				{
					delete mat.texture;
					mat.texture = nullptr;
				}

				renderer->ResetAccumulator();
			}

			if (!mat.texture)
			{
				if (ImGui::DragFloat3("Mat Color", &mat.color[0]))
					renderer->ResetAccumulator();
			}

			if (ImGui::DragFloat("Mat Spec", &mat.specularity, 0.05f, 0.0f, 1.0f))
				renderer->ResetAccumulator();
			if (ImGui::DragFloat("Mat Ref", &mat.refractance, 0.05f, 1.0f, 2.5f))
				renderer->ResetAccumulator();
			if (ImGui::DragFloat("Mat Rough", &mat.roughness, 0.05f, 0.0f, 1.0f))
				renderer->ResetAccumulator();
			if (ImGui::InputInt("Mat Type", (int*)&mat.type))
				renderer->ResetAccumulator();
		}

		if (ImGui::Checkbox("Bilinear interpolation", &Settings::bilinearInterpolation))
			renderer->ResetAccumulator();

		if (ImGui::Checkbox("Multisampling", &Settings::multisampled))
			renderer->ResetAccumulator();

		if (ImGui::Checkbox("MipMapping", &Settings::mipmapping))
			renderer->ResetAccumulator();
		if (ImGui::Checkbox("Denoise", &Settings::denoise))
			renderer->ResetAccumulator();
		if (ImGui::Checkbox("Stochastic lights", &Settings::stochasticLights))
			renderer->ResetAccumulator();
		if (ImGui::SliderFloat("Fov", &Settings::fov, 0.0f, 90.0f))
			renderer->ResetAccumulator();
		if (ImGui::SliderFloat("Skybox Intensity", &renderLevel->skybox.intensity, 0.0f, 5.0f))
			renderer->ResetAccumulator();

		ImGui::End();
	}

	void DebugController::Tick(float)
	{
		if (IsKeyDown(GLFW_KEY_R))
		{
			// Select primitive by mouse.
			Ray r = Game::Get()->GetRenderer()->GetCamera()->GetPrimaryRay((float)Game::Get()->GetMousePos().x, (float)Game::Get()->GetMousePos().y);
			renderLevel->FindNearest(r);
			if (r.object)
			{
				selectedPrimitive = r.object;
				selectedVoxel = r.voxelIndex;


				const int matIndex = selectedPrimitive->type == PrimitiveType::Voxel ?
					selectedPrimitive->grid[selectedVoxel] : selectedPrimitive->materialIndex;
				Material& mat = renderLevel->materials[matIndex];

				if (mat.texture)
				{
					inputText = mat.texture->FilePath();
				}
				else
				{
					inputText = "";
				}

				Game::Get()->GetRenderer()->ResetAccumulator();
			}
		}
	}
}