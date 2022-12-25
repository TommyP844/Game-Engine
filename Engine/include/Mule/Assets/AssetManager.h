#pragma once

#include "Mule/Ref.h"
#include "Asset.h"

#include <string>
#include <vector>

namespace Mule::AssetManager
{
	void UnloadAllAssets();

	void TrackAsset(const std::wstring& name, Ref<Asset> asset);

	void UnTrackAsset(const std::wstring& name);

	Ref<Asset> GetAsset(const std::wstring& name);

	std::vector<std::wstring> GetLoadedAssets();
}
