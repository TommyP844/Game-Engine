#include "mulepch.h"

#include "Mule/Assets/AssetManager.h"
#include "Mule/Util/Log.h"

namespace Mule::AssetManager
{
	struct AssetData
	{
		std::map<std::wstring, Ref<Asset>> Assets;
	};

	static AssetData sAssetData;

	void UnloadAllAssets()
	{
		LOG_WARN(L"AssetManager unloading all assets");
		sAssetData.Assets.clear();
	}

	void TrackAsset(const std::wstring& name, Ref<Asset> asset)
	{
#ifdef DEBUG
		LOG_MSG(L"AssetManager now tracking: {0}", name);
		if (sAssetData.Assets.find(name) != sAssetData.Assets.end())
		{
			LOG_ERR(L"Asset name: {0} exists");
			return;
		}
#endif
		sAssetData.Assets[name] = asset;
	}

	void UnTrackAsset(const std::wstring& name)
	{
#ifdef DEBUG
		LOG_MSG(L"AssetManager no longer tracking: {0}", name);
		if (sAssetData.Assets.find(name) != sAssetData.Assets.end())
		{
			LOG_ERR(L"Asset name: {0} does not exist", name);
			return;
		}
#endif
		sAssetData.Assets.erase(name);
	}

	Ref<Asset> GetAsset(const std::wstring& name)
	{
#ifdef DEBUG
		if (sAssetData.Assets.find(name) != sAssetData.Assets.end())
		{
			LOG_ERR(L"Asset name: {0} does not exist", name);
			return nullptr;
		}
#endif
		return sAssetData.Assets[name];
	}

	std::vector<std::wstring> GetLoadedAssets()
	{
		std::vector<std::wstring> list;
		for (auto& iter : sAssetData.Assets)
		{
			list.push_back(iter.first);
		}
		return list;
	}



}