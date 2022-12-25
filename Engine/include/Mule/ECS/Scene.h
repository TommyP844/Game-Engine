#pragma once

#include "entt/entt.hpp"
#include "Mule/Ref.h"
#include "GUID.h"
#include "Entity.h"
#include <string>
#include <filesystem>
#include <functional>


namespace Mule {

	class Scene
	{
	public:
		Scene();
		~Scene();
		static Ref<Scene> Create(const std::wstring& name = L"Scene");

		const std::wstring& GetName() const;

		/*
		* Checks whether the current scene is loaded, scenes that are loaded asyncrounusly
		* should not be accessed until this returns true
		*/
		bool IsLoaded() const { return mIsLoaded; }

		static void Serialize(const std::filesystem::path& filepath, Ref<Scene> scene);
		static void SerializeAsync(const std::filesystem::path& filepath, Ref<Scene> scene);
		static Ref<Scene> DeSerialize(const std::filesystem::path& filepath);
		static Ref<Scene> DeSerializeAsync(const std::filesystem::path& filepath);

		// Entity Functions

		Entity CreateEntity(const std::wstring& tag = L"Entity");
		Entity CreateEntity(const std::wstring& tag, GUID guid);

		void RemoveEntity(Entity e);

		template<typename F>
		void ForEach(F func);

		uint64_t GetNumEntites() const;

		// Entity Functions
		bool IsEntityValid(entt::entity id);

	private:
		friend class Mule::Entity;
		friend class Entity;

		bool mIsLoaded;
		std::wstring mName;
		WeakRef<Scene> mWeakThis;
		entt::basic_registry<entt::entity> mRegistry;
		std::vector<std::wstring> mAssetsToLoad;

		static void DeSerializeInternal(const std::filesystem::path& filepath, Ref<Scene> scene);

		template<typename T, typename ... Args>
		inline T& AddComponent(entt::entity id, Args&&... args);

		template<typename T>
		inline T& GetComponent(entt::entity id);

		template<typename T>
		inline bool HasComponent(entt::entity id);

		template<typename T>
		inline void RemoveComponent(entt::entity id);
	};

	template<typename T, typename ... Args>
	inline T& Scene::AddComponent(entt::entity id, Args&&... args)
	{
		return mRegistry.emplace<T>(id, std::forward<Args>(args)...);
	}

	template<typename T>
	inline T& Scene::GetComponent(entt::entity id)
	{
		return mRegistry.get<T>(id);
	}
	template<typename T>
	inline bool Scene::HasComponent(entt::entity id)
	{
		return mRegistry.all_of<T>(id);
	}
	template<typename T>
	inline void Scene::RemoveComponent(entt::entity id)
	{
		mRegistry.remove<T>(id);
	}

	template<typename F>
	inline void Scene::ForEach(F func)
	{
		mRegistry.each(func);
	}

}