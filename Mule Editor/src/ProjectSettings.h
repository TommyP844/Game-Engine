#pragma once

#include "Mule.h"

class ProjectSettings
{
public:
	ProjectSettings(){}
	~ProjectSettings(){}

	void Load(const fs::path& path);
	void Save();

	const fs::path& Path() const { return mPath; }

private:
	fs::path mPath;
	fs::path mAssetMapFile;
	fs::path mProjectSettingsFile;
	std::thread mAssetLoadingThread;
};