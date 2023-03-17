#include "mulepch.h"

#include "Mule/Assets/AssetManager.h"


namespace Mule
{
	AssetManager* AssetManager::sInstance = nullptr;

	AssetType AssetManager::GetTypeFromExtension(const std::string& extension)
	{
		if (extension == ".png" || extension == ".bmp" || extension == ".tga" || extension == ".jpg" || extension == ".jpeg")
			return Asset_Texture;
		if (extension == ".scene")
			return Asset_Scene;
		if (extension == ".obj" || extension == ".dae" || extension == ".fbx")
			return Asset_MeshCollection;
		if (extension == ".shader")
			return Asset_Shader;
		if (extension == ".material")
			return Asset_Material;

		return Asset_Invalid;
	}

	AssetManager::AssetEntry& AssetManager::GetAssetWithMetaData(const std::string& name)
	{
		return mAssetDB[name];
	}

	bool AssetManager::IsAssetTracked(const std::string& name)
	{
		return mAssetDB.find(name) != mAssetDB.end();
	}

	void AssetManager::TrackAsset(const std::string& name, Ref<void> asset, const fs::path& path, AssetType type)
	{
		AssetEntry ae;
		ae.Asset = asset;
		ae.Name = name;
		ae.Type = type;
		ae.Path = path;
		mAssetDB[name] = ae;
	}

	void AssetManager::UnloadAsset(const std::string& name)
	{
		mAssetDB.erase(name);
	}

	void AssetManager::ChangeAssetName(const std::string& old, const std::string& newName)
	{
		AssetEntry ae = mAssetDB[old];
		mAssetDB.erase(old);
		mAssetDB[newName] = ae;
	}

	std::vector<std::string> AssetManager::GetAssetNamesOfType(AssetType assetType)
	{
		std::vector<std::string> names;

		for (auto& it : mAssetDB)
		{
			if (it.second.Type == assetType)
			{
				names.push_back(it.first);
			}
		}

		return names;
	}

	void AssetManager::Init(const fs::path root)
	{
		if (sInstance != nullptr)
		{
			LOG_WARN(L"AssetManager already initialized");
			return;
		}
		sInstance = new AssetManager();
		sInstance->mRootDir = root;
	}

	void AssetManager::Shutdown()
	{
		if (!sInstance)
		{
			LOG_WARN(L"Shutdown() called on uninitialized AssetManager");
			return;
		}
		delete sInstance;
	}

}