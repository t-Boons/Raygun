#include "precomp.h"
#include "lightManipulator.h"

namespace Gameplay
{
	void LightManipulator::Init()
	{
		std::vector<PointLight>& lights = renderLevel->pointLights;
		lights.reserve(3);

		lights[0].position.x = 1;
		lights[2].position.z = 1;
	}

	void LightManipulator::TickUI()
	{
		ImGui::SetNextWindowPos(ImVec2(0.0f, ImGui::GetIO().DisplaySize.y - ImGui::GetIO().DisplaySize.y * 0.175f));
		ImGui::SetNextWindowSize({ ImGui::GetIO().DisplaySize.x , ImGui::GetIO().DisplaySize.y * 0.175f });


		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
		ImGui::Begin("Light positions", false, windowFlags);

		ImGui::Text("Match the ligth shadow colors with the little red, green or ");
		ImGui::Text("blue line shown on the screen by moving the sliders.");

		ImGui::PushItemWidth(ImGui::GetIO().DisplaySize.x);

		// Render UI to change light positions.
		for (uint32_t i = 0; i < renderLevel->pointLights.size(); i++)
		{
			float2 pos = GetLightPosition3Dto2D(i);
			if (ImGui::SliderFloat2((std::string("##Light pos ") + std::to_string(i)).c_str(), &pos[0], 0.0f, 1.0f))
			{
				SetLightPosition2Dto3D(pos, i);
				Game::Get()->GetRenderer()->ResetAccumulator();
			}
			ImGui::Spacing();
		}

		ImGui::End();
	}

	void LightManipulator::Tick(float deltaTime)
	{

		// Reset the lights.
		if (!hasReset)
		{
			for (uint32_t i = 0; i < numLights; i++)
			{
				SetLightPosition2Dto3D(lerp(GetLightPosition3Dto2D(i), resetLightPositions[i], transitionSpeed * deltaTime * 0.01f), i);
				Game::Get()->GetRenderer()->ResetAccumulator();
			}

			bool finishedResetting = true;
			for (uint32_t  i = 0; i < numLights; i++)
			{
				if(length((GetLightPosition3Dto2D(i) - resetLightPositions[i])) > 0.01f)
				{
					finishedResetting = false;
				}
			}

			if (finishedResetting)
			{
				hasReset = true;
			}
		}
	}

	void LightManipulator::ResetLights()
	{
		hasReset = false;
	}

	void LightManipulator::SetLightPosition2Dto3D(const float2& pos, int index)
	{
		std::vector<PointLight>& lights = renderLevel->pointLights;
		lights[index].position = { pos.x, 1.0f, pos.y};
	}

	float2 LightManipulator::GetLightPosition3Dto2D(int index)
	{
		const std::vector<PointLight>& lights = renderLevel->pointLights;
		return { lights[index].position.x, lights[index].position.z };
	}
}