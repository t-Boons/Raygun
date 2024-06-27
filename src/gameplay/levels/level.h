#pragma once
#include "gameplay/entity.h"

namespace Gameplay
{
	class Level
	{
	public:
		virtual void Init() = 0;
		
		virtual void Start();
		virtual void Tick(float deltaTime);

		virtual void TickUI();

		virtual ~Level();

		template<typename T>
		T* GetEntity()
		{
			for (auto& entt : entities)
			{
				T* found = dynamic_cast<T*>(entt);
				if (found) return found;
			}

			return nullptr;
		}

		const std::vector<Entity*>& GetEntities() const { return entities; }

	protected:
		std::vector<Entity*> entities;
	};
}