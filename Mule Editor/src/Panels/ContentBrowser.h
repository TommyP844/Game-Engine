#pragma once

#include "Panel.h"
#include "Layers/EditorLayer.h"
#include "Util.h"
#include <set>

#include <thread>
#include <atomic>

class ContentBrowser : public Panel
{
public:
	ContentBrowser(Mule::Ref<EditorState> editorState)
		:
		Panel("Content Browser", editorState)
	{}

	virtual void OnAttach() override;
	virtual void OnUpdate(float dt) override;
	virtual void OnDetach() override;
	virtual void OnEvent(Mule::Ref<Mule::Event> event) override;
	virtual void OnImGuiRender() override;

private:

	fs::path mAssetPath;
	fs::path mBrowserDirectory;

	// Used for renaming files in the editor
	fs::path mRenamePath;
	char mRenameBuffer[256] = { 0 };

	char mSearchBuffer[256] = { 0 };
	std::string mSearchString;
	bool mShouldSearch = false;

	const float mFileWidth = 150.f;

	bool DisplayFile(const fs::directory_entry& entry);
	bool FuzzySearch(const std::string& s1, const std::string& s2, int threshold);

	std::map<Mule::AssetHandle, Mule::AssetHandle> mMaterialPreviewTextures;
};