#include "precomp.h"
#include "level.h"


namespace Gameplay
{
	void Level::Start()
	{
		for (Entity*& entt : entities)
		{
			entt->Start();
		}
	}

	void Level::Tick(float deltaTime)
	{
		for (Entity*& entt : entities)
		{
			entt->Tick(deltaTime);
		}
	}

	void Level::TickUI()
	{
		for (Entity*& entt : entities)
		{
			entt->TickUI();
		}
	}

	Level::~Level()
	{
		for (Entity*& entt : entities)
		{
			delete entt;
		}
	}
}