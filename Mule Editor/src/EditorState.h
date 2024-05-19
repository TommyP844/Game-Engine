#pragma once

#include "Mule.h"
#include <filesystem>
#include <atomic>
#include <set>

namespace fs = std::filesystem;

struct EditorState
{
	fs::path ProjectPath;
	std::set<Mule::Entity> SelectedEntities;
	std::atomic<bool> AssetsLoaded = false;

	Mule::Ref<Mule::Scene> GetActiveScene()
	{
		Mule::AssetManager& manager = Mule::AssetManager::Get();
		return manager.GetAsset<Mule::Scene>(mSceneHandleAtomic.load());
	}

	void SetActiveSceneHandle(Mule::AssetHandle handle)
	{
		mSceneHandleAtomic.store(handle);
	}

	Mule::AssetHandle GetActiveSceneHandle()
	{
		return mSceneHandleAtomic.load();
	}

private:
	std::atomic<Mule::AssetHandle> mSceneHandleAtomic;
	
};

void GenerateEditorState();
void DestoyEditorState();
EditorState& GetEditorState();
