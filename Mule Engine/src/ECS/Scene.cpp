#include "ECS/Scene.h"
#include "ECS/Entity.h"
#include "ECS/Components.h"
#include "Mule Assert.h"
#include "Serialization/yaml.h"
#include "Asset/AssetManager.h"

#include <entt/entt.hpp>

#include <fstream>

namespace Mule
{
	Scene::Scene(AssetHandle handle, const fs::path& directory)
		:
		Asset(handle, directory, AssetType::Scene)
	{
	}

	Scene::Scene(const fs::path& directory)
		:
		Asset(Asset::GenerateHandle(), directory, AssetType::Scene)
	{
	}

	void Scene::Serialize(SerializationMode mode)
	{
		switch (mode)
		{
		case SerializationMode::Text:
			SerializeText();
			break;
		case SerializationMode::Binary:
			SerializeBinary();
			break;
		default:
			MULE_LOG_ERROR("Invalid serialization mode: {0}", (int)mode);
			MULE_ASSERT(false);
			break;
		}
		ClearModified();
	}

	Ref<Scene> Scene::Deserialize(const fs::path& filepath, SerializationMode mode)
	{
		Ref<Scene> scene = Ref<Scene>(new Scene(filepath));
		switch (mode)
		{
		case SerializationMode::Text: DeserializeText(scene, filepath); break;
		case SerializationMode::Binary:	DeserializeBinary(scene, filepath); break;
		default:
			MULE_LOG_ERROR("Failed to deserialize scene: {0}, with mode: {1}", filepath, (int)mode);
			break;
		}
		return scene;
	}
	
	Ref<Scene> Scene::DeserializeAsync(const fs::path& filepath, SerializationMode mode)
	{
		Ref<Scene> scene = Ref<Scene>(new Scene(filepath));
		switch (mode)
		{
		case SerializationMode::Text: break;
		case SerializationMode::Binary:	DeserializeBinary(scene, filepath); break;
		default:
			MULE_LOG_ERROR("Failed to deserialize scene: {0}, with mode: {1}", filepath, (int)mode);
			break;
		}
		return scene;
	}

	Entity Scene::LoadEntity(entt::entity id, const std::string& tag, const Guid& guid)
	{
		entt::entity entityId = mRegistry.create(id);

		AddComponent<TagComponent>(entityId).Tag = tag;
		AddComponent<GuidComponent>(entityId).Guid = guid;
		AddComponent<RelationshipComponent>(entityId);
		AddComponent<TransformComponent>(entityId);

		return Entity(entityId, WeakRef<Scene>(this));
	}

	Entity Scene::CreateEntity(const std::string& tag, const Guid& guid)
	{
		SetModified();

		entt::entity id = mRegistry.create();

		AddComponent<TagComponent>(id, tag);
		AddComponent<GuidComponent>(id);
		AddComponent<RelationshipComponent>(id);
		AddComponent<TransformComponent>(id);

		return Entity(id, WeakRef<Scene>(this));
	}

	void Scene::DestroyEntity(entt::entity id)
	{
		mRegistry.destroy(id);
	}

	void Scene::RemoveEntity(Entity e)
	{
		mRegistry.destroy((entt::entity)e.ID());
	}

	void Scene::IterateEntities(std::function<void(Entity)> func)
	{
		auto view = mRegistry.view<TagComponent>();
			
		for(auto id : view) 
		{
			func(Entity(id, WeakRef<Scene>(this)));
		}
	}

	void Scene::IterateParentEntities(std::function<void(Entity)> func)
	{
		auto view = mRegistry.view<TagComponent>();

		for(auto id : view)
		{
			Entity e(id, WeakRef<Scene>(this));
			if (!e.IsChild())
			{
				func(e);
			}
		}
	}
	
	bool Scene::IsEntityValid(entt::entity id)
	{
		return mRegistry.valid(id);
	}

	void Scene::OnUpdate(float dt)
	{
		auto cameraView = mRegistry.view<CameraComponent>();
		for (auto id : cameraView)
		{
			auto& camera = GetComponent<CameraComponent>(id);
			auto& transform = GetComponent<TransformComponent>(id);
			camera.Camera.UpdateMatrices(transform.Transform.TR());
		}
	}

	void Scene::OnRender(float dt)
	{
		
	}

	// TODO: Add editor camera
	void Scene::OnEditorRender(float dt, const std::set<Entity>& highlightedEntities, const Camera& editorCamera)
	{
		
	}

	void Scene::SerializeText()
	{
		YAML::Node sceneNode;
		sceneNode["Guid"] = Handle();
		IterateParentEntities([&](Mule::Entity e) {
			sceneNode["Entities"].push_back(SerializeEntityText(e));
			});

		fs::path filepath = Filepath();
		if (!fs::exists(filepath.parent_path()))
		{
			fs::create_directories(filepath.parent_path());
		}
		MULE_LOG_VERBOSE("Saving Scene to file: {0}", filepath);
		YAML::Emitter emitter;
		emitter << sceneNode;
		std::ofstream file(filepath.string());
		if (!file.is_open())
		{
			MULE_LOG_ERROR("Failed to open file: {0}", filepath);
			return;
		}
		file << emitter.c_str();
		file.close();
	}
	
	void Scene::SerializeBinary()
	{
	}

	void Scene::DeserializeText(Ref<Scene> scene, const fs::path& filepath)
	{
		YAML::Node root = YAML::LoadFile(filepath.string());

		AssetHandle handle = root["Guid"].as<AssetHandle>();
		scene->SetHandle(handle);

		for (auto entity : root["Entities"])
		{
			DeserializeEntityText(entity, scene, Entity());
		}
	}

#define DESERIALIZE_COMPONENT(type) \
	if (componentNode["Name"].as<std::string>() == #type) \
	{ \
		e.AddComponent<type>() = componentNode.as<type>(); \
		continue; \
	}

	Entity Scene::DeserializeEntityText(const YAML::Node& node, Ref<Scene> scene, Entity parent)
	{
		std::string tag = node["Tag"].as<std::string>();
		uint32_t id = node["Id"].as<uint32_t>();
		Guid guid = node["Guid"].as<Guid>();
		Transform transform = node["Transform"].as<Transform>();

		Entity e = scene->LoadEntity((entt::entity)id, tag, guid);
		e.GetComponent<TransformComponent>().Transform = transform;

		if (parent)
		{
			parent.AddChild(e);
		}

		if (node["Components"])
		{
			for (auto componentNode : node["Components"])
			{
				DESERIALIZE_COMPONENT(CameraComponent);
				DESERIALIZE_COMPONENT(ModelComponent);
				DESERIALIZE_COMPONENT(PointLightComponent);
				DESERIALIZE_COMPONENT(SpotLightComponent);
				DESERIALIZE_COMPONENT(DirectionalLightComponent);
			}
		}

		if (node["Children"])
		{
			for (const auto& child : node["Children"])
			{
				DeserializeEntityText(child, scene, e);
			}
		}

		return e;
	}

	void Scene::DeserializeBinary(Ref<Scene> scene, const fs::path& filepath)
	{
	}

	YAML::Node Scene::SerializeEntityText(Mule::Entity e)
	{
		YAML::Node entity;
		entity["Tag"] = e.Tag();
		entity["Id"] = e.ID();
		entity["Guid"] = e.Guid();
		entity["Transform"] = e.Transform();

#define SERIALIZE_COMPONENT(type) if (e.HasComponent<type>()) entity["Components"].push_back(e.GetComponent<type>());
	
		SERIALIZE_COMPONENT(ModelComponent);
		SERIALIZE_COMPONENT(CameraComponent);
		SERIALIZE_COMPONENT(PointLightComponent);
		SERIALIZE_COMPONENT(SpotLightComponent);
		SERIALIZE_COMPONENT(DirectionalLightComponent);

		for (auto& child : e.Children())
		{
			entity["Children"].push_back(SerializeEntityText(child));
		}

		return entity;
	}

}