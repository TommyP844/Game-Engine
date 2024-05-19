#include "EditorState.h"
#include "ProjectSettings.h"

#include "Mule.h"


#include <fstream>

void ProjectSettings::Load(const fs::path& path)
{
	mPath = path;
	
	GenerateEditorState();
	
	mProjectSettingsFile = mPath / "ProjectSettings.yml";
	mAssetMapFile = mPath / "AssetMapping.yml";
	if (fs::exists(mAssetMapFile))
	{
		Mule::AssetManager& manager = Mule::AssetManager::Get();

		EditorState& state = GetEditorState();
		if (fs::exists(mProjectSettingsFile))
		{
			YAML::Node root = YAML::LoadFile(mAssetMapFile.string());
			if (root)
			{
				if (root["ActiveScene"])
				{
					state.SetActiveSceneHandle(root["ActiveScene"].as<Mule::AssetHandle>());
				}
			}
		}

		if (fs::exists(mAssetMapFile))
		{
			YAML::Node root = YAML::LoadFile(mAssetMapFile.string());

			if (root["Textures"])
			{
				for (auto assetNode : root["Textures"])
				{
					Mule::AssetHandle handle = assetNode["Handle"].as<Mule::AssetHandle>();
					fs::path path = assetNode["Path"].as<std::string>();

					if (!fs::exists(path))
						continue;

					//auto texture = Mule::Texture::Load2D(handle, path);
					//manager.InsertAsset(texture);
				}
			}

			if (root["Materials"])
			{
				for (auto assetNode : root["Materials"])
				{
					Mule::AssetHandle handle = assetNode["Handle"].as<Mule::AssetHandle>();
					fs::path path = assetNode["Path"].as<std::string>();

					if (!fs::exists(path))
						continue;

					//auto material = Mule::Material::Load(path);
					//manager.InsertAsset(material);
				}
			}

			if (root["Models"])
			{
				for (auto assetNode : root["Models"])
				{
					Mule::AssetHandle handle = assetNode["Handle"].as<Mule::AssetHandle>();
					fs::path path = assetNode["Path"].as<std::string>();

					std::map<std::string, Mule::AssetHandle> meshes;

					for (auto meshNode : assetNode["Meshes"])
					{
						std::string name = meshNode["Name"].as<std::string>();
						Mule::AssetHandle meshHandle = meshNode["Handle"].as<Mule::AssetHandle>();
						meshes[name] = meshHandle;
					}

					if (!fs::exists(path))
						continue;

					//auto modelDesc = Mule::LoadModel(handle, meshes, path);
					//manager.InsertAsset(modelDesc);
				}
			}

			if (root["Scenes"])
			{
				for (auto assetNode : root["Scenes"])
				{
					Mule::AssetHandle handle = assetNode["Handle"].as<Mule::AssetHandle>();
					fs::path path = assetNode["Path"].as<std::string>();

					if (!fs::exists(path))
						continue;

					// this will always be slow because we are creating the rendersystem and shader loading is slow
					//auto scene = Mule::Scene::Deserialize(path, Mule::SerializationMode::Text);
					//manager.InsertAsset(scene);
				}
			}
		}

		GetEditorState().AssetsLoaded = true;
		MULE_LOG_SUCCESS("Assets Loaded");
	}
	else
	{
		GetEditorState().AssetsLoaded = true;
	}
}

void ProjectSettings::Save()
{
	// Asset mapping
	{
		YAML::Node root;
		Mule::AssetManager& manager = Mule::AssetManager::Get();
		for (auto [handle, asset] : manager.GetAllAssets())
		{
			YAML::Node assetNode;
			assetNode["Handle"] = handle;
			assetNode["Path"] = asset->Filepath().string();

			switch (asset->Type())
			{
			case Mule::AssetType::Scene:
				root["Scenes"].push_back(assetNode);
				break;
			case Mule::AssetType::Material:
				root["Materials"].push_back(assetNode);
				break;
			case Mule::AssetType::Model:
			{
				Mule::Ref<Mule::ModelDesc> model = asset;
				for (auto meshHandle : model->MeshHandles)
				{
					YAML::Node meshNode;
					Mule::Ref<Mule::Mesh> mesh = manager.GetAsset<Mule::Mesh>(meshHandle);

					meshNode["Handle"] = meshHandle;
					meshNode["Name"] = mesh->Name();

					assetNode["Meshes"].push_back(meshNode);
				}
				root["Models"].push_back(assetNode);
			}
				break;
			case Mule::AssetType::Shader:
				root["Shaders"].push_back(assetNode);
				break;
			case Mule::AssetType::Texture:
				root["Textures"].push_back(assetNode);
				break;
			}
		}

		YAML::Emitter emitter;
		emitter << root;
		std::ofstream file(mAssetMapFile, std::ios::trunc | std::ios::out);
		file << emitter.c_str();
		file.close();
	}
	

	// Project Settings
	{
		YAML::Node root;

		root["ActiveScene"] = GetEditorState().GetActiveSceneHandle();

		YAML::Emitter emitter;
		emitter << root;
		std::ofstream file(mProjectSettingsFile, std::ios::trunc | std::ios::out);
		file << emitter.c_str();
		file.close();
	}
}
