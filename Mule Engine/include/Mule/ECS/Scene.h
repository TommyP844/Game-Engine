#pragma once

#include "Ref.h"
#include "GUID.h"
#include "Asset/Asset.h"

#include <entt/entt.hpp>
#include <yaml-cpp/yaml.h>
#include "Graphics/Camera.h"

#include <string>
#include <set>

namespace Mule
{
	class Entity;

	struct SceneRenderSettings
	{
		bool Shadows = true;

	};

	class Scene : public Asset
	{
	public:
		static Ref<Scene> LoadScene(AssetHandle handle, const fs::path& directory)
		{
			return Ref<Scene>(new Scene(handle, directory));
		}

		static Ref<Scene> Create(const fs::path& directory)
		{
			return Ref<Scene>(new Scene(directory));
		}

		void Serialize(SerializationMode mode);
		static Ref<Scene> Deserialize(const fs::path& filepath, SerializationMode mode);
		static Ref<Scene> DeserializeAsync(const fs::path& filepath, SerializationMode mode);

		~Scene() {}

		Entity LoadEntity(entt::entity id, const std::string& tag, const Guid& guid);
		Entity CreateEntity(const std::string& tag = "Entity", const Guid& guid = Guid());

		void DestroyEntity(entt::entity id);
		void RemoveEntity(Entity e);
		void IterateEntities(std::function<void(Entity)> func);

		void IterateParentEntities(std::function<void(Entity)> func);


		template<typename T, typename ...Args>
		T& AddComponent(entt::entity id, Args&&... args)
		{
			SetModified();
			mRegistry.emplace<T>(id, std::forward<Args>(args)...);
			return mRegistry.get<T>(id);
		}

		template<typename T>
		T& GetComponent(entt::entity id)
		{
			return mRegistry.get<T>(id);
		}

		template<typename T>
		void RemoveComponent(entt::entity id)
		{
			SetModified();
			mRegistry.remove<T>(id);
		}

		template<typename T>
		bool HasComponent(entt::entity id)
		{
			return mRegistry.all_of<T>(id);
		}

		bool IsEntityValid(entt::entity id);

		void OnUpdate(float dt);

		SceneRenderSettings& RenderSettings() { return mSettings; }
		entt::registry& GetRegistry() { return mRegistry; }

	private:
		entt::registry mRegistry;
		SceneRenderSettings mSettings;

		Scene(AssetHandle handle, const fs::path& directory);
		Scene(const fs::path& directory);


		void SerializeText();
		void SerializeBinary();
		static void DeserializeText(Ref<Scene> scene, const fs::path& filepath);
		static Entity DeserializeEntityText(const YAML::Node& node, Ref<Scene> scene, Entity parent);
		static void DeserializeBinary(Ref<Scene> scene, const fs::path& filepath);

		YAML::Node SerializeEntityText(Mule::Entity e);

	};
}
