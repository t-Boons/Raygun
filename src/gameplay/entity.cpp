#include "precomp.h"
#include "entity.h"


namespace Gameplay
{
	Entity::Entity(Level* lvl)
		: level(lvl)
	{
		renderLevel = Game::Get()->GetRenderer()->GetScene();
	}

	void Entity::AddToRenderlevel(Primitive primitive)
	{
		renderable = &renderLevel->objects[renderLevel->GetObjectCount()];
		renderLevel->AddObject(primitive, { 0,0,0 }, { 0,0,0 }, { 1,1,1 });

		renderable;
	}

	float3 Entity::GetPosition() const
	{
		return renderLevel->transforms[renderable->matrixIndex].position; 
	}

	float3 Entity::GetRotation() const
	{
		return renderLevel->transforms[renderable->matrixIndex].rotation;
	}

	float3 Entity::GetScale() const
	{
		return renderLevel->transforms[renderable->matrixIndex].scale;
	}

	void Entity::SetPosition(const float3& position)
	{
		renderLevel->transforms[renderable->matrixIndex].position = position;
		renderLevel->transforms[renderable->matrixIndex].ToMatrix();
	}

	void Entity::SetRotation(const float3& rotation)
	{
		renderLevel->transforms[renderable->matrixIndex].rotation = rotation;
		renderLevel->transforms[renderable->matrixIndex].ToMatrix();
	}

	void Entity::SetScale(const float3& scale)
	{
		renderLevel->transforms[renderable->matrixIndex].scale = scale;
		renderLevel->transforms[renderable->matrixIndex].ToMatrix();
	}
}