#include "precomp.h"
#include "gameplayLevel.h"
#include "gameplay/gameManager.h"
#include "gameplay/lightManipulator.h"

namespace Gameplay
{
	void GameplayLevel::Init()
	{
		{
			GameManager* entt = new GameManager(this);
			entities.push_back(entt);
		}

		{
			LightManipulator* entt = new LightManipulator(this);
			entities.push_back(entt);
		}

		for (auto& entt : entities)
		{
			entt->Init();
		}
	}
}