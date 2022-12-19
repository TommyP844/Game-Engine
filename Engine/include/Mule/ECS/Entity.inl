
#include "Mule/Ecs/Entity.h"

#include "Mule/ECS/Scene.h"

template<typename T, typename ... Args>
inline T& Entity::AddComponent(Args&&... args)
{
	mRegistry.emplace<T>(id, args);
	// TODO: insert return statement here
}

template<typename T>
inline bool Entity::HasComponent()
{
	mScene->mRegistry
	return false;
}

template<typename T>
inline T& Entity::GetComponent()
{
	// TODO: insert return statement here
}

template<typename T>
inline void Entity::RemoveComponent()
{
}
