#pragma once

#include "Mule.h"

#include <filesystem>
#include <set>
#include <atomic>

namespace fs = std::filesystem;

class EditorState
{
public:
	EditorState(const fs::path& projectPath);
	~EditorState();

	void SaveState();

	std::atomic<Mule::AssetHandle> ActiveSceneHandle;
	std::set<Mule::Entity> SelectedEntities;
	Mule::Ref<Mule::GraphicsDevice> Device;
	
	
	Mule::Ref<Mule::Scene> GetActiveScene()
	{
		Mule::AssetManager& manager = Mule::AssetManager::Get();
		return manager.GetAsset<Mule::Scene>(ActiveSceneHandle);
	}

	const fs::path& GetProjectPath() const { return mProjectPath; }
	const fs::path& GetAssetsPath() const { return mAssetsPath; }


private:
	fs::path mAssetsPath;
	fs::path mProjectPath;
	fs::path mAssetMappingFile;
	fs::path mEditorStateFile;
};