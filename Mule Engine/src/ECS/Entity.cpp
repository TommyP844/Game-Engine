#include "ECS/Entity.h"

#include "ECS/Components.h"

namespace Mule
{
	const std::string& Entity::Tag()
	{
		return GetComponent<MetaComponent>().Tag;
	}

	Guid Entity::Guid()
	{
		return GetComponent<MetaComponent>().Guid;
	}

	Entity Entity::Parent()
	{
		auto& relationship = GetComponent<MetaComponent>();
		return relationship.Parent;
	}

	std::vector<Entity> Entity::Children()
	{
		auto& relationship = GetComponent<MetaComponent>();
		return std::vector<Mule::Entity>(relationship.Children.begin(), relationship.Children.end());
	}

	void Entity::Orphan()
	{
		if(!HasComponent<RootComponent>())
			AddComponent<RootComponent>();
		auto& relationship = GetComponent<MetaComponent>();

		if (relationship.Parent)
		{
			auto& parentRelationship = relationship.Parent.GetComponent<MetaComponent>();
			if (parentRelationship.Children.contains(Entity(mId, mScene)))
			{
				parentRelationship.Children.erase(Entity(mId, mScene));
			}
		}
		
		relationship.Parent = Entity();
	}

	void Entity::RemoveChild(Entity child)
	{
		child.GetComponent<MetaComponent>().Parent = Entity();

		auto& relationship = GetComponent<MetaComponent>();
		relationship.Children.erase(child);
	}

	void Entity::AddChild(Entity child) 
	{
		child.Orphan();

		if(child.HasComponent<RootComponent>())
			child.RemoveComponent<RootComponent>();

		auto& childRelationship = child.GetComponent<MetaComponent>();
		childRelationship.Parent = Entity(mId, mScene);

		auto& relationship = GetComponent<MetaComponent>();
		relationship.Children.insert(child);
	}

	bool Entity::IsChild()
	{
		auto& relComp = GetComponent<MetaComponent>();
		return relComp.Parent;
	}
	
	Transform& Entity::Transform()
	{
		return GetComponent<MetaComponent>().Transform;
	}

	void Entity::Destroy()
	{
		mScene->DestroyEntity(mId);
		mId = entt::null;
		mScene = nullptr;
	}
}