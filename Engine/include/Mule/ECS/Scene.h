#pragma once

#include "entt/entt.hpp"
#include "Mule/Ref.h"
#include "GUID.h"
#include "Entity.h"
#include "imgui.h"
#include <string>
#include <filesystem>
#include <functional>
#include "Mule/Renderer/Framebuffer.h"


namespace Mule {

	class Scene
	{
	public:
		Scene();
		~Scene();

		Scene(const Scene& other) = default;

		static Ref<Scene> Create(const std::string& name = "Scene");

		const std::string& GetName() const;

		/*
		* Checks whether the current scene is loaded, scenes that are loaded asyncrounusly
		* should not be accessed until this returns true
		*/
		bool IsLoaded() const { return mIsLoaded; }

		static void Serialize(const std::filesystem::path& filepath, Ref<Scene> scene);
		static Ref<Scene> DeSerialize(const std::filesystem::path& filepath);

		void SetModified(bool modified);
		bool IsModified() const { return mIsModified; }

		const fs::path& GetPath() const { return mPath; }

		// Entity Functions

		Entity CreateEntity(const std::string& tag = "Entity");
		Entity CreateEntity(const std::string& tag, GUID guid);

		void RemoveEntity(Entity e);

		template<typename F>
		void ForEach(F func);

		uint64_t GetNumEntites() const;

		std::vector<Entity> GetAllEntities() const;
		std::vector<Entity> GetParentEntities() const;

		// Entity Functions
		bool IsEntityValid(entt::entity id);

		// Runtime
		void InitRenderer(int width, int height);
		void ResizeFramebuffer(int width, int height);

		ImTextureID OnEditorRender(int width, int height);

	private:
		friend class Mule::Entity;
		friend class Entity;

		bool mIsModified;
		bool mIsLoaded;
		fs::path mPath;
		std::string mName;
		WeakRef<Scene> mWeakThis;
		entt::basic_registry<entt::entity> mRegistry;
		std::vector<std::string> mAssetsToLoad;
		Ref<FrameBuffer> mEditorFrameBuffer;

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
		mIsModified = true;
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
		mIsModified = true;
		mRegistry.remove<T>(id);
	}

	template<typename F>
	inline void Scene::ForEach(F func)
	{
		mRegistry.each(func);
	}

}