#pragma once

#include <vector>

#include <entt/entt.hpp>

#include "Mule/Ref.h"
#include "GUID.h"


namespace Mule {

	class Scene;

	class Entity
	{
	public:
		Entity();
		Entity(entt::entity id, WeakRef<Scene> scene);


		// Components

		template<typename T, typename ... Args>
		inline T& AddComponent(Args&&... args);

		template<typename T>
		inline bool HasComponent() const;

		template<typename T>
		inline T& GetComponent();

		template<typename T>
		inline const T& GetComponent() const;

		template<typename T>
		inline void RemoveComponent();

		// Relationships

		Entity GetParent();

		std::vector<Entity> GetChildren();

		void AddChild(Entity e);
		void RemoveChild(Entity e);

		// Convienence

		entt::entity GetID() const;

		WeakRef<Scene> GetScene() const;

		const std::wstring& GetTag() const;

		GUID GetGUID() const;

		// Operators


	private:

		entt::entity mID;
		Ref<Scene> mScene; // TODO: make a weak ref
	};

	
	template<typename T, typename ... Args>
	inline T& Entity::AddComponent(Args&&... args)
	{
		return mScene->AddComponent<T>(mID, std::forward<Args>(args)...);
	}

	template<typename T>
	inline bool Entity::HasComponent() const
	{
		return mScene->HasComponent<T>(mID);
	}

	template<typename T>
	inline T& Entity::GetComponent()
	{
		return mScene->GetComponent<T>(mID);
	}

	template<typename T>
	inline const T& Entity::GetComponent() const
	{
		return mScene->GetComponent<T>(mID);
	}

	template<typename T>
	inline void Entity::RemoveComponent()
	{
		mScene->RemoveComponent<T>(mID);
	}

}