#include "mulepch.h"

#include "Mule/ECS/Entity.h"
#include "Mule/ECS/Components.h"
#include "Mule/ECS/Scene.h"

namespace Mule {
	
	Entity::Entity()
		:
		mID(entt::null),
		mScene(Ref<Scene>())
	{}

	Entity::Entity(entt::entity id, WeakRef<Scene> scene)
		:
		mID(id),
		mScene(scene)
	{
	}

	Entity Entity::GetParent()
	{
		return Entity();
	}

	std::vector<Entity> Entity::GetChildren()
	{
		return std::vector<Entity>();
	}

	void Entity::AddChild(Entity e)
	{
	}

	void Entity::RemoveChild(Entity e)
	{
	}

	entt::entity Entity::GetID() const
	{
		return mID;
	}

	WeakRef<Scene> Entity::GetScene() const
	{
		return mScene;
	}

	const std::wstring& Entity::GetTag() const
	{
		return GetComponent<TagComponent>().Tag;
	}

	GUID Entity::GetGUID() const
	{
		return GetComponent<GUIDComponent>().GUID;
	}
	
}
