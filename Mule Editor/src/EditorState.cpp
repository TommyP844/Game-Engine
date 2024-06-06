#include "EditorState.h"

#include <yaml-cpp/yaml.h>
#include <yaml-cpp/emitter.h>

#include <fstream>

EditorState::EditorState(const fs::path& projectPath)
	:
	mProjectPath(projectPath)
{
	if (!fs::exists(mProjectPath))
	{
		fs::create_directory(mProjectPath);
	}
	mAssetsPath = mProjectPath / "Assets";
	if (!fs::exists(mAssetsPath))
	{
		fs::create_directory(mAssetsPath);
	}

	mAssetMappingFile = mProjectPath / "AssetMapping.yml";

	if (fs::exists(mAssetMappingFile))
	{
		Mule::AssetManager& manager = Mule::AssetManager::Get();
		YAML::Node root = YAML::LoadFile(mAssetMappingFile.string());

		if (root["Scenes"])
		{
			YAML::Node scenes = root["Scenes"];
			for (auto& sceneNode : scenes.as<std::vector<YAML::Node>>())
			{
				Mule::AssetHandle handle = sceneNode["Handle"].as<Mule::AssetHandle>();
				fs::path filepath = sceneNode["FilePath"].as<std::string>();

				Mule::Ref<Mule::Scene> scene = Mule::Scene::Deserialize(filepath, Mule::SerializationMode::Text);
				manager.InsertAsset(scene);
			}
		}
	}

	mEditorStateFile = mProjectPath / "EditorState.yml";
	if (fs::exists(mEditorStateFile))
	{
		YAML::Node root = YAML::LoadFile(mEditorStateFile.string());

		if (root["ActiveSceneHandle"])
		{
			ActiveSceneHandle = root["ActiveSceneHandle"].as<Mule::AssetHandle>();
		}
	}
}

EditorState::~EditorState()
{
}

void EditorState::SaveState()
{
	// Asset Mapping
	{
		Mule::AssetManager& manager = Mule::AssetManager::Get();
		YAML::Node root;

		YAML::Node scenes = root["Scenes"];
		for (Mule::Ref<Mule::Scene> scene : manager.GetAllAssetsOfType(Mule::AssetType::Scene))
		{
			if (!fs::exists(scene->Filepath()))
				continue;

			YAML::Node sceneNode;

			sceneNode["Handle"] = scene->Handle();
			sceneNode["FilePath"] = scene->Filepath().string();

			scenes.push_back(sceneNode);
		}

		std::ofstream assetMapFile(mAssetMappingFile);
		if (assetMapFile.is_open())
		{
			YAML::Emitter emitter;
			emitter << root;
			assetMapFile << emitter.c_str();
			assetMapFile.close();
		}
	}

	// Editor State
	{
		YAML::Node root;

		root["ActiveSceneHandle"] = (Mule::AssetHandle)ActiveSceneHandle;

		std::ofstream editorStateFile(mEditorStateFile);
		if (editorStateFile.is_open())
		{
			YAML::Emitter emitter;
			emitter << root;
			editorStateFile << emitter.c_str();
			editorStateFile.close();
		}
	}
}
