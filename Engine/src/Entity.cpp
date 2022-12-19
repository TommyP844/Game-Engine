#include "mulepch.h"

#include "Mule/ECS/Entity.h"
#include "Mule/ECS/Scene.h"

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
