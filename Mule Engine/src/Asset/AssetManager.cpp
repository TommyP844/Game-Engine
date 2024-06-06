#include "Asset/AssetManager.h"
#include <fstream>
#include <set>

namespace Mule
{
	void AssetManager::InsertAsset(Ref<Asset> asset)
	{
		std::lock_guard<std::mutex> lock(mMutex);

		if (mAssets.find(asset->Handle()) != mAssets.end())
		{
			MULE_LOG_ERROR("Asset id already being tracked: {0}", asset->Handle());
			//MULE_ASSERT(false);
		}
		mAssets[asset->Handle()] = asset;
	}

	void AssetManager::RemoveAsset(AssetHandle handle)
	{
		std::lock_guard<std::mutex> lock(mMutex);

		if (mAssets.find(handle) == mAssets.end())
		{
			MULE_LOG_ERROR("Failed to find asset with id: {0}", handle);
			return;
		}
		mAssets.erase(handle);
	}

	std::vector<Ref<Asset>> AssetManager::GetAllAssetsOfType(AssetType type)
	{
		//std::lock_guard<std::mutex> lock(mMutex);

		std::vector<Ref<Asset>> assets;
		for (auto& [handle, asset] : mAssets)
		{
			if (asset->Type() == type)
			{
				assets.push_back(asset);
			}
		}
		return assets;
	}

	bool AssetManager::DoesAssetExist(const fs::path& path, AssetType type)
	{
		std::lock_guard<std::mutex> lock(mMutex);

		std::vector<Ref<Asset>> assets;
		for (auto& [handle, asset] : mAssets)
		{
			if (asset->Type() != type)
			{
				continue;
			}
			if (asset->Filepath() == path)
				return true;
		}
		return false;
	}

	void AssetManager::IterateAssets(std::function<void(WeakRef<Asset>)> func)
	{
		std::lock_guard<std::mutex> lock(mMutex);

		for (auto& [handle, asset] : mAssets)
		{
			func(asset);
		}
	}

	AssetManager::AssetManager()
		:
		mAssets({})
	{
		
	}

	AssetManager::~AssetManager()
	{
	}

	AssetType AssetManager::GetTypeFromPath(const fs::path& path)
	{
		std::string ext = path.filename().extension().string();

		const std::set<std::string> textureExtensions = { ".png", ".tga", ".jpg", ".jpeg", ".bmp" };
		const std::set<std::string> meshExtensions = { ".dae", ".fbx", ".blend", ".obj" };

		if (ext == ".scene")
			return AssetType::Scene;
		if (textureExtensions.find(ext) != textureExtensions.end())
			return AssetType::Texture;
		if (meshExtensions.find(ext) != meshExtensions.end())
			return AssetType::Mesh;
		if (ext == ".mshader")
			return AssetType::Shader;
		if (ext == ".mmaterial")
			return AssetType::Material;

		return AssetType::None;
	}
}