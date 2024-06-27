#pragma once
#include "entity.h"

namespace Gameplay
{
	class DebugController : public Entity
	{
	public:
		DebugController(Level* lvl)
			: Entity(lvl)
		{}

		virtual void Init() override;
		virtual void TickUI() override;
		virtual void Tick(float deltaTime) override;

	private:
		const Primitive* selectedPrimitive = nullptr;
		int selectedVoxel = 0;
		std::string inputText;
		int selectedRenderingMode = 0;
	};
}