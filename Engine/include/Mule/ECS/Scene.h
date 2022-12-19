#pragma once

#include "entt/entt.hpp"

class Entity;

class Scene
{
public:
	~Scene();

	Entity CreateEntity();
	void RemoveEntity(Entity e);

private:
	friend class Entity;
	Scene();


	

	entt::basic_registry<entt::entity> mRegistry;
};