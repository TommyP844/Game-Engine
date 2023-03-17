#pragma once

#include "Mule/Mule.h"

#include "IPanel.h"

#include <filesystem>

namespace fs = std::filesystem;


class ContentBrowserPanel : public IPanel
{
public:
	ContentBrowserPanel(AppData& appData, const std::string& baseDir);
	~ContentBrowserPanel();

	virtual void OnAttach() override;
	virtual void OnUpdate() override;
	virtual void OnDetach() override;

private:
	fs::path mAssetsDir;
	fs::path mCurrentDir;

	struct DirEntry
	{
		std::string Name;
		bool IsDir;
		fs::path FullPath;
	};

	std::vector<DirEntry> mFiles;

	void UpdateDir();

	const char* GetPayloadFromExtension(const std::string& ext);

};

