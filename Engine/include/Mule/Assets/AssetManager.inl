#pragma once


namespace Mule
{
	template<typename T>
	Ref<T> AssetManager::GetAsset(const std::string& name)
	{
		return std::static_pointer_cast<T>(mAssetDB[name].Asset);
	}

	template<typename T>
	inline std::vector<AssetManager::AssetEntry> AssetManager::GetAssetsOfType(AssetType type)
	{
		std::vector<AssetEntry> assets;

		for (auto& it : mAssetDB)
		{
			if (it.second.Type == type)
			{
				assets.push_back(it.second);
			}
		}

		return assets;
	}
}