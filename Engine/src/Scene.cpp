#include "mulepch.h"
#include "Mule/ECS/Scene.h"

#include "yaml-cpp/yaml.h"
#include "Mule/ECS/Components.h"
#include "Mule/Internal/ThreadPool.h"
#include "Mule/Util/Log.h"
#include "Mule/ECS/Serializer.h"
#include "Mule/Assets/AssetManager.h"

#include "bgfx/bgfx.h"
#include "bgfx/defines.h"
#include "bgfx/platform.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Mule/Renderer/Renderer.h"

#define COMPONENTS "components"

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

	Ref<Scene> Scene::Create(const std::string& name)
	{
		Ref<Scene> scene = MakeRef<Scene>();
		scene->mWeakThis = scene;
		scene->mName = name;

		return scene;
	}

	const std::string& Scene::GetName() const
	{
		return mName;
	}

	void Scene::Serialize(const std::filesystem::path& filepath, Ref<Scene> scene)
	{
		scene->mIsModified = false;
		YAML::Node root;
		YAML::Node entities = root["Entities"];

		scene->mRegistry.each([scene, &root, &entities](entt::entity id) {
			Mule::Entity entity(id, scene);

			YAML::Node enode;

			YAML::Node components = enode[COMPONENTS];

			if (entity.HasComponent<TagComponent>())
			{
				components["TAG"] = SerializeTag(entity.GetComponent<TagComponent>());
			}
			if (entity.HasComponent<GUIDComponent>())
			{
				components["GUID"] = SerializeGuid(entity.GetComponent<GUIDComponent>());
			}
			if (entity.HasComponent<TransformComponent>())
			{
				components["TRANSFORM"] = SerializeTransform(entity.GetComponent<TransformComponent>());
			}


			entities.push_back(enode);

			});
		std::ofstream file(filepath);
		if (!file.is_open())
		{
			LOG_ERR(L"Failed to open file \"{0}\"", filepath.string());
			return;
		}
		file << root;
		file.close();
	}

	Ref<Scene> Scene::DeSerialize(const std::filesystem::path& filepath)
	{
		Ref<Scene> scene = Create();
		scene->mPath = filepath;
		YAML::Node root = YAML::LoadFile(filepath.u8string());
		
		for (auto entityNode : root["Entities"])
		{ 
			Entity entity(scene->mRegistry.create(), scene);


		
		}
		scene->mIsLoaded = true;
		return scene;
	}

	void Scene::SetModified(bool modified)
	{
		mIsModified = true;
	}

	Entity Scene::CreateEntity(const std::string& tag)
	{
		mIsModified = true;
		auto id = mRegistry.create();
		Entity entity(id, mWeakThis);
		entity.AddComponent<TagComponent>(tag);
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<GUIDComponent>();
		entity.AddComponent<RelationshipComponent>();
		return entity;
	}

	Entity Scene::CreateEntity(const std::string& tag, GUID guid)
	{
		mIsModified = true;
		Entity entity(mRegistry.create(), mWeakThis);
		entity.AddComponent<TagComponent>(tag);
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<GUIDComponent>(guid);
		entity.AddComponent<RelationshipComponent>();
		return entity;
	}

	void Scene::RemoveEntity(Entity e)
	{
		mIsModified = true;
		for (auto child : e.GetChildren())
		{
			// this will set the children of this entity to be children of the parent entity if it exists
			if (e.GetParent().IsValid())
			{
				child.GetComponent<RelationshipComponent>().Parent = e.GetParent();
				e.GetParent().AddChild(child);
			}
			else
			{
				child.GetComponent<RelationshipComponent>().Parent = Entity();
			}
		}

		// remove entity from parent
		Entity parent = e.GetParent();
		if (parent.IsValid())
		{
			auto& pchildren = parent.GetComponent<RelationshipComponent>().Children;
			auto iter = std::find(pchildren.begin(), pchildren.end(), e);
			if (iter != pchildren.end())
			{
				pchildren.erase(iter);
			}
			else
			{
				LOG_ERR(L"Failed to remove child from parent");
			}
		}
		
		mRegistry.destroy(e.GetID());
	}

	uint64_t Scene::GetNumEntites() const
	{
		return mRegistry.alive();
	}

	std::vector<Entity> Scene::GetAllEntities() const
	{
		std::vector<Entity> entities;
		mRegistry.each([&](entt::entity id) {
			Entity entity(id, mWeakThis);
		entities.push_back(entity);
			});
		return entities;
	}

	std::vector<Entity> Scene::GetParentEntities() const
	{
		std::vector<Entity> entities;
		mRegistry.each([&](entt::entity id) 
			{
				Entity entity(id, mWeakThis);
				if(!entity.GetParent().IsValid())
					entities.push_back(entity);
			});
		return entities;
	}

	bool Scene::IsEntityValid(entt::entity id)
	{
		return mRegistry.valid(id);
	}

	void Scene::InitRenderer(int width, int height)
	{
		FrameBufferInfo info;
		info.Attachments = { RGBA8, D32F };
		info.Width = width;
		info.Height = height;
		mEditorFrameBuffer = FrameBuffer::Create(info);
	}

	void Scene::ResizeFramebuffer(int width, int height)
	{
		mEditorFrameBuffer->Resize(width, height);
	}

	ImTextureID Scene::OnEditorRender(int width, int height)
	{
		CameraComponent camera;
		
		glm::mat4 proj = glm::perspective(glm::radians(80.f), (float)width / (float)height, 1.f, 1000.f);
		glm::mat4 view = glm::lookAt(glm::vec3(-100.f, 100.f, -100.f), glm::vec3(0.f, -0.f, 0.f), glm::vec3(0.f, -1.f, 0.f));

		Renderer::BeginFrame(view, proj, mEditorFrameBuffer);

		AssetManager& manager = AssetManager::Get();

		auto objView = mRegistry.view<MeshCollectionComponent>();
		for (auto entity : objView)
		{
			Entity e(entity, mWeakThis);
			const TransformComponent& transform = e.GetComponent<TransformComponent>();
			const MeshCollectionComponent& collection = e.GetComponent<MeshCollectionComponent>();
			
			if (!collection.Material || !collection.Shader || !collection.Meshes || !collection.Visible)
				continue;
			
			glm::mat4 translation = glm::translate(glm::identity<glm::mat4>(), transform.Translation);
			glm::mat4 orientation = glm::toMat4(transform.Orientation);
			glm::mat4 scale = glm::scale(glm::identity<glm::mat4>(), transform.Scale);
			
			glm::mat4 model = translation * orientation * scale;

			Renderer::Submit(
				collection.Meshes,
				collection.Shader,
				collection.Material,
				transform.Translation,
				model, [&]() 
				{
					Ref<Material> mat = collection.Material;

					Ref<Texture> albedo = manager.GetAsset<Texture>(mat->AlbedoMap);
					Ref<Texture> normal = manager.GetAsset<Texture>(mat->NormalMap);
					Ref<Texture> metalic = manager.GetAsset<Texture>(mat->MetalnessMap);
					Ref<Texture> roughness = manager.GetAsset<Texture>(mat->RoughnessMap);
					Ref<Texture> ao = manager.GetAsset<Texture>(mat->AOMap);

					if (albedo) collection.Shader->SetTexture("AlbedoMap", albedo);
					if (normal) collection.Shader->SetTexture("NormalMap", normal);
					if (metalic) collection.Shader->SetTexture("MetalnessMap", metalic);
					if (roughness) collection.Shader->SetTexture("RoughnessMap", roughness);
					if (ao) collection.Shader->SetTexture("AOMap", ao);
				});
		}

		Renderer::Render();

		Renderer::EndFrame();
		

		return mEditorFrameBuffer->GetAttachment(0);
	}

}
