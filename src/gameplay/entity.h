#pragma once
#include "gameplay/levels/level.h" 
#include "renderer/scene.h"

namespace Gameplay
{
	class Level;
	class Entity
	{
	public:
		Entity(Level* lvl);
		virtual ~Entity() = default;

		void AddToRenderlevel(Primitive primitive);

		virtual void Init() = 0;
		virtual void Start() {}
		virtual void Tick(float) {}
		virtual void TickUI() {}

		float3 GetPosition() const;
		float3 GetRotation() const;
		float3 GetScale() const;

		void SetPosition(const float3& position);
		void SetRotation(const float3& rotation);
		void SetScale(const float3& scale);

		Primitive* GetPrimitive() { return renderable; }

	protected:
		Primitive* renderable;
		Level* level;
		RenderLevel* renderLevel;
	};
}