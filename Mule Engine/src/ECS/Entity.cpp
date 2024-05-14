#include "ECS/Entity.h"

#include "ECS/Components.h"

namespace Mule
{
	const std::string& Entity::Tag()
	{
		return GetComponent<TagComponent>().Tag;
	}

	Guid Entity::Guid()
	{
		return GetComponent<GuidComponent>().Guid;
	}

	Entity Entity::Parent()
	{
		auto& relationship = GetComponent<RelationshipComponent>();
		return relationship.Parent;
	}

	const std::vector<Entity>& Entity::Children()
	{
		auto& relationship = GetComponent<RelationshipComponent>();
		return relationship.Children;
	}

	void Entity::Orphan()
	{
		auto& relationship = GetComponent<RelationshipComponent>();

		if (relationship.Parent)
		{
			auto& parentRelationship = relationship.Parent.GetComponent<RelationshipComponent>();
			auto iter = std::find(parentRelationship.Children.begin(), parentRelationship.Children.end(), Entity(mId, mScene));
			if (iter != parentRelationship.Children.end())
			{
				parentRelationship.Children.erase(iter);
			}
		}
		
		relationship.Parent = Entity();
	}

	void Entity::RemoveChild(Entity child)
	{
		child.GetComponent<RelationshipComponent>().Parent = Entity();

		auto& relationship = GetComponent<RelationshipComponent>();
		auto iter = std::find(relationship.Children.begin(), relationship.Children.end(), (Entity(mId, mScene)));
		relationship.Children.erase(iter);
	}

	void Entity::AddChild(Entity child)
	{
		auto& childRelationship = child.GetComponent<RelationshipComponent>();
		childRelationship.Parent = Entity(mId, mScene);

		auto& relationship = GetComponent<RelationshipComponent>();
		relationship.Children.push_back(child);
	}

	bool Entity::IsChild()
	{
		auto& relComp = GetComponent<RelationshipComponent>();
		return relComp.Parent;
	}
	
	Transform& Entity::Transform()
	{
		return GetComponent<TransformComponent>().Transform;
	}

	void Entity::Destroy()
	{
		mScene->DestroyEntity(mId);
		mId = entt::null;
		mScene = nullptr;
	}
}