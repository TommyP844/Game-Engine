#include "mulepch.h"

#include "Mule/ECS/Scene.h"
#include "Mule/ECS/Components.h"
#include "Mule/Internal/ThreadPool.h"
#include "Mule/ECS/YamlConvert.h"
#include "Mule/Util/Log.h"



namespace Mule 
{
	Scene::Scene()
		:
		mIsLoaded(false)
	{
	}
	Scene::~Scene()
	{
	}

	Ref<Scene> Scene::Create(const std::wstring& name)
	{
		Ref<Scene> scene = MakeRef<Scene>();
		scene->mWeakThis = scene;
		scene->mName = name;
		return scene;
	}

	const std::wstring& Scene::GetName() const
	{
		return mName;
	}

	void Scene::Serialize(const std::filesystem::path& filepath, Ref<Scene> scene)
	{
		YAML::Node root;
		scene->mRegistry.each([scene, &root](entt::entity id) {
			Mule::Entity entity(id, scene);
			root["Entities"].push_back(entity);
			});
		std::ofstream file(filepath);
		if (!file.is_open())
		{
			LOG_ERR(L"Failed to open file \"{0}\"", filepath.wstring());
			return;
		}
		file << root;
		file.close();
	}

	void Scene::SerializeAsync(const std::filesystem::path& filepath, Ref<Scene> scene)
	{
		Internal::ThreadPool::SubmitTask([filepath, scene]() {
			Serialize(filepath, scene);
			});
	}

	void Scene::DeSerializeInternal(const std::filesystem::path& filepath, Ref<Scene> scene)
	{
		YAML::Node root = YAML::LoadFile(filepath.u8string());

		for (auto entityNode : root["Entities"])
		{
			Entity entity(scene->mRegistry.create(), scene);

			YAML::Node components = entityNode[COMPONENTS];
			if (components[TAG_COMPONENT])
				entity.AddComponent<Mule::TagComponent>(components[TAG_COMPONENT].as<Mule::TagComponent>());
			if (components[GUID_COMPONENT])
				entity.AddComponent<Mule::GUIDComponent>(components[GUID_COMPONENT].as<Mule::GUIDComponent>());
			if (components[TRANSFORM_COMPONENT])
				entity.AddComponent<Mule::TransformComponent>(components[TRANSFORM_COMPONENT].as<Mule::TransformComponent>());
		}
		scene->mIsLoaded = true;
	}

	Ref<Scene> Scene::DeSerialize(const std::filesystem::path& filepath)
	{
		Ref<Scene> scene = Create();
		DeSerializeInternal(filepath, scene);
		return scene;
	}

	Ref<Scene> Scene::DeSerializeAsync(const std::filesystem::path& filepath)
	{
		Ref<Scene> scene = Create();
		Internal::ThreadPool::SubmitTask([filepath, scene]() {
			DeSerializeInternal(filepath, scene);
			});
		return scene;
	}

	Entity Scene::CreateEntity(const std::wstring& tag)
	{
		auto id = mRegistry.create();
		Entity entity(id, mWeakThis);
		entity.AddComponent<TagComponent>(tag);
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<GUIDComponent>();
		return entity;
	}

	Entity Scene::CreateEntity(const std::wstring& tag, GUID guid)
	{
		Entity entity(mRegistry.create(), mWeakThis);
		entity.AddComponent<TagComponent>(tag);
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<GUIDComponent>(guid);
		return entity;
	}

	void Scene::RemoveEntity(Entity e)
	{
		mRegistry.destroy(e.GetID());
	}

	uint64_t Scene::GetNumEntites() const
	{
		return 0;
	}

	bool Scene::IsEntityValid(entt::entity id)
	{
		return mRegistry.valid(id);
	}

}
