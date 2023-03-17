#include "mulepch.h"

#include "Mule/ECS/Entity.h"
#include "Mule/ECS/Components.h"
#include "Mule/ECS/Scene.h"
#include "Mule/Util/Log.h"

namespace Mule {
	
	Entity::Entity()
		:
		mID(entt::null),
		mScene(nullptr)
	{}

	Entity::Entity(entt::entity id, WeakRef<Scene> scene)
		:
		mID(id),
		mScene(scene)
	{
	}

	bool Entity::HasChildren() const
	{
		return GetComponent<RelationshipComponent>().Children.size() > 0;
	}

	bool Entity::IsChild() const
	{
		return GetComponent<RelationshipComponent>().Parent.IsValid();
	}

	Entity Entity::GetParent() const
	{
		return GetComponent<RelationshipComponent>().Parent;
	}

	std::vector<Entity> Entity::GetChildren() const
	{
		return GetComponent<RelationshipComponent>().Children;
	}

	void Entity::AddChild(Entity e)
	{
		mScene->SetModified(true);
		auto& children = GetComponent<RelationshipComponent>().Children;
		children.push_back(e);
		e.GetComponent<RelationshipComponent>().Parent = Entity(mID, mScene);
	}

	void Entity::RemoveChild(Entity e)
	{
		mScene->SetModified(true);
		e.GetComponent<RelationshipComponent>().Parent = Entity();
		auto& children = GetComponent<RelationshipComponent>().Children;
		auto iter = std::find(children.begin(), children.end(), e);
		if (iter != children.end())
		{
			children.erase(iter);
			return;
		}
		LOG_ERR(L"Entity: {0} not a child of Entity: {1}", e.GetTag(), GetTag());
	}

	entt::entity Entity::GetID() const
	{
		return mID;
	}

	Ref<Scene> Entity::GetScene() const
	{
		return mScene;
	}

	const std::string& Entity::GetTag() const
	{
		return GetComponent<TagComponent>().Tag;
	}

	GUID Entity::GetGUID() const
	{
		return GetComponent<GUIDComponent>().GUID;
	}

	bool Entity::IsValid() const
	{
		return mID != entt::null && mScene;
	}
	
}
