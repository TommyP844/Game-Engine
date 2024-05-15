#pragma once

#include "Util/Singleton.h"
#include "Asset/Asset.h"
#include "Ref.h"
#include "Log.h"

#include <map>
#include <random>
#include <thread>
#include <mutex>

namespace Mule
{
	class AssetManager : public Util::Singleton<AssetManager>
	{
	public:
		void InsertAsset(Ref<Asset> asset);

		template<typename T>
		Ref<T> GetAsset(AssetHandle handle)
		{
			std::lock_guard<std::mutex> lock(mMutex);

			if (mAssets.find(handle) == mAssets.end())
			{
				return nullptr;
			}
			return mAssets[handle];
		}

		void RemoveAsset(AssetHandle handle);

		std::vector<Ref<Asset>> GetAllAssetsOfType(AssetType type);

		bool DoesAssetExist(const fs::path& path, AssetType type);
		
		template<typename T>
		Ref<T> GetAssetByFilepath(const fs::path& path)
		{
			std::lock_guard<std::mutex> lock(mMutex);
			for (auto& [handle, asset] : mAssets)
			{
				if (path == asset->Filepath())
					return asset;
			}
			return nullptr;
		}

		const std::map<AssetHandle, Ref<Asset>>& GetAllAssets() const { return mAssets; }

		AssetManager();
		virtual ~AssetManager();
	private:
		std::mutex mMutex;

		std::map<AssetHandle, Ref<Asset>> mAssets;
		fs::path mAssetFile;

		AssetHandle GenerateUUID()
		{
			std::random_device rd;
			std::mt19937_64 gen(rd());
			std::uniform_int_distribution<size_t> dis;

			return dis(gen);
		}

		static AssetType GetTypeFromPath(const fs::path& path);
	};
}