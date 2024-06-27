#pragma once

namespace Gameplay
{
	class LightManipulator : public Entity
	{
	public:
		LightManipulator(Level* lvl)
			: Entity(lvl)
		{}

		virtual void Init() override;
		virtual void TickUI() override;
		virtual void Tick(float deltaTime) override;

		void ResetLights();
		void SetLightPosition2Dto3D(const float2& pos, int index);
		float2 GetLightPosition3Dto2D(int index);

	private:
		bool hasReset = false;
		const float transitionSpeed = 0.35f;

		static const uint32_t numLights = 3;
		const float2 resetLightPositions[numLights] = { {0.05f, 0.95f}, {0.05f, 0.05f}, {0.95f, 0.05f} };
	};
}