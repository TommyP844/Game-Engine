#pragma once

#include "Mule.h"

class NewAssetPopUp
{
public:
	static Mule::Ref<NewAssetPopUp> Create(const std::string& name,
		const fs::path& assetDirectory,
		const fs::path& defaultDirectory,
		const std::string assetExtension);

	bool OnImGuiRender(fs::path& path);

private:
	NewAssetPopUp(const std::string& name,
		const fs::path& assetDirectory,
		const fs::path& defaultDirectory,
		const std::string assetExtension);

	std::string mName;
	fs::path mAssetDir;
	fs::path mDirectory;
	std::string mExtension;

	bool mShowPopup;
	char mInputBuffer[256];
};