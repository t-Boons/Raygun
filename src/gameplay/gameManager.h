#pragma once
#include "entity.h"

namespace Gameplay
{
	enum class Color
	{
		Red, Green, Blue
	};

	class GameManager : public Entity
	{
	public:
		GameManager(Level* lvl)
			: Entity(lvl), random(Random((uint32_t)time(NULL)))
		{}

		virtual void Init() override;
		virtual void Tick(float deltaTime) override;

		void ResetLevel();

		inline uint32_t ColorToColor32(Color color) const
		{
			if (color == Color::Red) return COLOR32(220, 0, 0);
			if (color == Color::Blue) return COLOR32(0, 0, 220);
			if (color == Color::Green) return COLOR32(0, 220, 0);
			return 0;
		}

	private:
		bool isOn = false;
		float tickTime = 0;
		std::vector<int2> currentPositions;
		int2 randomPoint;
		Color randomColor;
		Random random;
	};
}