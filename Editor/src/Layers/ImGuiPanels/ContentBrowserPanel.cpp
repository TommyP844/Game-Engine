#include "ContentBrowserPanel.h"

#include "imgui.h"

#include "../../ImGuiGlobals.h"

#include <filesystem>

namespace fs = std::filesystem;

ContentBrowserPanel::ContentBrowserPanel(AppData& appData, const std::string& baseDir)
	:
	IPanel("Assets Dir", appData),
	mAssetsDir(baseDir),
	mCurrentDir(baseDir)
{

}

ContentBrowserPanel::~ContentBrowserPanel()
{
}

void ContentBrowserPanel::OnAttach()
{
	UpdateDir();
}

void ContentBrowserPanel::OnUpdate()
{
	if (ImGui::Begin("Assets Dir"))
	{
		Mule::AssetManager& manager = Mule::AssetManager::Get();

		ImGui::Text("Dir: %s", mCurrentDir.relative_path().u8string().c_str());
		if (ImGui::Button("Back"))
		{
			if (mCurrentDir != mAssetsDir)
				mCurrentDir = mCurrentDir.parent_path();
			UpdateDir();
		}
		ImGui::Separator();

		bool updateFiles = false;
		for (auto& de : mFiles)
		{
			ImVec2 pos = ImGui::GetCursorPos();
			
			ImVec2 size = ImGui::GetContentRegionAvail();
			bool sameLine = true;
			if (pos.x + 240 > pos.x + size.x)
				sameLine = false;
			if (de.IsDir)
			{				
				if (ImGui::ImageButtonWithText(mAppData.EditorTextures["FolderIcon"]->GetImguiID(), de.Name.c_str(), {120, 120}))
				{
					mCurrentDir = de.FullPath;
					updateFiles = true;
				}
			}
			else
			{
				ImTextureID id = mAppData.EditorTextures["FileIcon"]->GetImguiID();

				std::string ext = de.FullPath.extension().u8string();
				const char* payload = GetPayloadFromExtension(ext);
				if (payload == PAYLOAD_TEXTURE)
				{
					id = manager.GetAsset<Mule::Texture>(de.Name)->GetImguiID();
				}

				if (ImGui::ImageButtonWithText(id, de.Name.c_str(), {120, 120}))
				{
				}
				if (ImGui::BeginDragDropSource())
				{
					ImGui::SetDragDropPayload(payload, de.Name.data(), de.Name.size(), ImGuiCond_Always);
					ImGui::EndDragDropSource();
				}
				if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					if (payload == PAYLOAD_SCENE)
					{
						// push scene panel
					}
					if (payload == PAYLOAD_MATERIAL)
					{
						// push material panel
					}
				}
			}
			if (sameLine) ImGui::SameLine();
		}
		// this is needed because update will destroy all current name pointers and potentially make the loop invalid when mFiles resizes with new files
		if (updateFiles)
		{
			UpdateDir();
		}
	}
	ImGui::End();
}

void ContentBrowserPanel::OnDetach()
{
}

void ContentBrowserPanel::UpdateDir()
{
	mFiles.clear();
	for (auto path : fs::directory_iterator(mCurrentDir))
	{
		DirEntry de;
		std::string name = path.path().filename().u8string();
		de.Name = name.substr(0, name.find_last_of("."));
		de.IsDir = path.is_directory();
		de.FullPath = path.path().u8string();
		mFiles.push_back(de);
	}
	std::sort(mFiles.begin(), mFiles.end(),
		[](DirEntry lhs, DirEntry rhs)
		{
			return (int)lhs.IsDir > (int)rhs.IsDir;
		});

	std::sort(mFiles.begin(), mFiles.end(),
		[](DirEntry lhs, DirEntry rhs)
		{
			if (lhs.IsDir != rhs.IsDir) return false;
			return lhs.Name < rhs.Name;
		});
}

const char* ContentBrowserPanel::GetPayloadFromExtension(const std::string& ext)
{
	if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp" || ext == ".tga")
		return PAYLOAD_TEXTURE;
	if (ext == ".scene")
		return PAYLOAD_SCENE;
	if (ext == ".material")
		return PAYLOAD_MATERIAL;
	
	return nullptr;
}

