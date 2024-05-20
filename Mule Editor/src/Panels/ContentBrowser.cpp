#include "ContentBrowser.h"

#include "imgui_internal.h"
#include "Layers/EditorLayer.h"
#include "Util.h"


#include <set>
#include <fstream>
#include <regex>

void ContentBrowser::OnAttach()
{
	//mAssetPath = mEditorState.ProjectPath / "Assets";
	//mBrowserDirectory = mAssetPath;
}

void ContentBrowser::OnUpdate(float dt)
{
	
}

void ContentBrowser::OnDetach()
{
}

void ContentBrowser::OnEvent(Mule::Ref<Mule::Event> event)
{
}

void ContentBrowser::OnImGuiRender()
{
	if (ImGui::Begin(Name().c_str()))
	{

		float width = ImGui::GetContentRegionAvail().x;
		float windowPadding = ImGui::GetStyle().WindowPadding.x;
		//if (ImGui::BeginChild("Folders", { width * 0.25f - 1.f * windowPadding, 0 }, ImGuiChildFlags_Border))
		//{
		//	ImGui::Text("Assets");
		//	ImGui::Separator();
		//	
		//	for (auto& path : fs::directory_iterator(mAssetPath))
		//	{
		//		if (path.is_directory())
		//		{
		//			float width = ImGui::GetContentRegionAvail().x;
		//			if (ImGui::Button(path.path().filename().string().c_str(), { width, 0 }))
		//			{
		//				mBrowserDirectory = path.path();
		//				mShouldSearch = false;
		//				memset(mSearchBuffer, 0, 256);
		//			}
		//		}
		//	}
		//}
		//ImGui::EndChild();
		//ImGui::SameLine();
		//if (ImGui::BeginChild("Content", { width * 0.75f, 0 }, ImGuiChildFlags_Border))
		//{
		//	auto assetDir = mBrowserDirectory.lexically_relative(mAssetPath.parent_path());
		//	ImGui::Text("Path:");
		//	ImGui::SameLine();
		//	
		//	fs::path iterativePath;
		//	for (auto& file : assetDir)
		//	{
		//		std::string breadCrumb = file == "." ? "Assets" : file.string();
		//		iterativePath /= file;
		//		ImGui::SameLine();
		//
		//		if (mBrowserDirectory.filename() == file)
		//		{
		//			ImGui::Text(breadCrumb.c_str());
		//		}
		//		else if (ImGui::Button(breadCrumb.c_str()))
		//		{
		//			//mBrowserDirectory = mEditorState.ProjectPath / iterativePath;
		//			//mBrowserDirectory = mAssetPath / iterativePath;
		//			mShouldSearch = false;
		//			memset(mSearchBuffer, 0, 256);
		//		}
		//
		//		ImGui::SameLine();
		//		ImGui::Text("\\");				
		//	}
		//
		//	ImGui::SameLine();
		//	ImGui::PushItemWidth(150.f);
		//	if (ImGui::InputText("Search", mSearchBuffer, 256))
		//	{
		//		mSearchString = mSearchBuffer;
		//		if (!mSearchString.empty())
		//		{
		//			mShouldSearch = true;
		//		}
		//		else
		//		{
		//			mShouldSearch = false;
		//		}
		//	}
		//
		//	ImGui::Separator();
		//	if (ImGui::BeginChild("Content-Data", ImGui::GetContentRegionAvail()))
		//	{
		//		if (ImGui::BeginPopupContextWindow("##RightClickContent"))
		//		{
		//			if (ImGui::BeginMenu("Create"))
		//			{
		//				if (ImGui::MenuItem("Material"))
		//				{
		//					std::string materialName = "Material.mmaterial";
		//					if (fs::exists(mBrowserDirectory / materialName))
		//						materialName = GenerateNewFilename(materialName, mBrowserDirectory);
		//
		//					//auto material = Mule::Material::Create(mBrowserDirectory / materialName);
		//					//material->Save();
		//				}
		//				if (ImGui::MenuItem("Folder"))
		//				{
		//					std::string folderName = "New Folder";
		//					if (fs::exists(mBrowserDirectory / folderName))
		//						folderName = GenerateNewFilename(folderName, mBrowserDirectory);
		//
		//					fs::create_directory(mBrowserDirectory / folderName);
		//				}
		//				ImGui::EndMenu();
		//			}
		//			ImGui::EndPopup();
		//		}
		//
		//		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 10, 10 });
		//		const float maxWidth = ImGui::GetContentRegionAvail().x;
		//		const float padding = ImGui::GetStyle().ItemSpacing.x;
		//		float total = mFileWidth;
		//		for (auto& dir : fs::directory_iterator(mBrowserDirectory))
		//		{
		//			if (mShouldSearch)
		//			{
		//				if (!FuzzySearch(mSearchString, dir.path().filename().string(), 3))
		//				{
		//					continue;
		//				}
		//			}
		//
		//			total += mFileWidth + padding;
		//
		//			bool doubleClicked = DisplayFile(dir);
		//			if (doubleClicked && dir.is_directory())
		//			{
		//				mBrowserDirectory = dir.path();
		//			}
		//			if (total >= maxWidth)
		//			{
		//				total = mFileWidth;
		//			}
		//			else
		//			{
		//				ImGui::SameLine();
		//			}
		//		}
		//		ImGui::PopStyleVar();
		//	}
		//	ImGui::EndChild();
		//}
		//ImGui::EndChild();
	}
	ImGui::End();
}

static ImU32 Vec4ToImCol32(const ImVec4& color) {
	return IM_COL32(
		static_cast<int>(color.x * 255.0f),
		static_cast<int>(color.y * 255.0f),
		static_cast<int>(color.z * 255.0f),
		static_cast<int>(color.w * 255.0f)
	);
}

bool ContentBrowser::DisplayFile(const fs::directory_entry& entry)
{
	FileType type = TypeFromPath(entry);

	const float borderWidth = 2.f;
	const float height = 200.f;
	const float textPadding = 5.f;
	ImVec4 tintColor = ImVec4(0, 0, 0, 0);
	const ImVec4 selectedColor = ImVec4(0.1f, 0.1f, 0.1f, 0.3f);
	
	ImGui::BeginGroup();

	auto cursor = ImGui::GetCursorScreenPos();

	bool ret = false;
	ImGui::InvisibleButton(entry.path().string().c_str(), { mFileWidth, height }, ImGuiButtonFlags_PressedOnClick | ImGuiButtonFlags_MouseButtonLeft);
	if (ImGui::IsItemHovered())
	{
		tintColor = ImVec4(0.1f, 0.1f, 0.1f, 0.2f);
	}
	if (ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left))
	{
		tintColor = ImVec4(0.1f, 0.1f, 0.1f, 0.3f);
	}

	ret = ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && ImGui::IsItemHovered();

	Mule::AssetManager& manager = Mule::AssetManager::Get();
	if (ImGui::BeginPopupContextItem(entry.path().filename().string().c_str()))
	{
		if (ImGui::MenuItem("Edit"))
		{
			switch (type)
			{
			case FileType::Shader:
			case FileType::Unknown:
				break;
			case FileType::Material:
			{
			}
				break;
			}
		}
		if (ImGui::MenuItem("Rename"))
		{
			mRenamePath = entry.path();
			memcpy(mRenameBuffer, mRenamePath.filename().string().c_str(), mRenamePath.filename().string().size());
		}
		if (ImGui::MenuItem("Delete"))
		{
			// todo: make a modal pop up
			fs::remove(entry.path());
		}
		ImGui::EndPopup();
	}

	ImTextureID id;

	ImVec4 borderCol = ImGui::GetStyle().Colors[ImGuiCol_Border];
	ImVec4 bgColor = ImGui::GetStyle().Colors[ImGuiCol_DockingEmptyBg];
	// outside
	ImGui::GetWindowDrawList()->AddRectFilled(cursor, ImVec2(cursor.x + mFileWidth, cursor.y + height), Vec4ToImCol32(bgColor), 10.f, ImDrawFlags_RoundCornersBottom);
	// border
	ImGui::GetWindowDrawList()->AddRect(cursor, ImVec2(cursor.x + mFileWidth, cursor.y + height), Vec4ToImCol32(borderCol), 10.f, ImDrawFlags_RoundCornersBottom, borderWidth);
	//ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(cursor.x + borderWidth, cursor.y + borderWidth), ImVec2(cursor.x + mFileWidth - borderWidth, cursor.y + mFileWidth - borderWidth), Vec4ToImCol32(bgColor));
	ImGui::GetWindowDrawList()->AddLine({ cursor.x, cursor.y + mFileWidth }, { cursor.x + mFileWidth, cursor.y + mFileWidth }, Vec4ToImCol32(borderCol), borderWidth);


	ImVec2 imageSize(mFileWidth - 2 * borderWidth, mFileWidth - 2 * borderWidth);
	ImGui::SetCursorScreenPos({ cursor.x + borderWidth, cursor.y + borderWidth });
	
	ImGui::Image(id, imageSize);
	
	// Display text
	ImGui::SetCursorScreenPos({ cursor.x + textPadding + borderWidth, cursor.y + mFileWidth + textPadding });

	if (mRenamePath == entry.path())
	{
		ImGui::SetNextItemWidth(mFileWidth - 2 * (textPadding + borderWidth));
		if (ImGui::InputText("##RenameFile",
			mRenameBuffer, 256,
			ImGuiInputTextFlags_EnterReturnsTrue))
		{
			// todo: handle file extension
			std::string ext = mRenamePath.extension().string();
			std::string newFilename = mRenameBuffer;
			
			if (!newFilename.ends_with(ext))
			{
				newFilename += ext;
			}

			const std::regex pattern("^[a-zA-Z0-9_\\.\\-\\(\\)]+$");
			if (std::regex_match(newFilename, pattern))
			{
				fs::rename(mRenamePath, mRenamePath.parent_path() / newFilename);
				mRenamePath = fs::path();
				memset(mRenameBuffer, 0, 256);
			}
		}
		else
		ImGui::SetKeyboardFocusHere(-1);
	}
	else
	{
		ImGui::RenderTextWrapped(
			{ cursor.x + textPadding, cursor.y + mFileWidth + textPadding },
			entry.path().filename().replace_extension().string().c_str(),
			nullptr,
			mFileWidth - 2 * borderWidth - textPadding);
		ImGui::GetWindowDrawList()->AddRectFilled(cursor, ImVec2(cursor.x + mFileWidth, cursor.y + height), Vec4ToImCol32(tintColor));
	}

	// If we are renaming and click away then cancel
	//if (!ImGui::IsItemFocused() && mRenamePath == entry.path())
	//{
	//	mRenamePath = fs::path();
	//	memset(mRenameBuffer, 0, 256);
	//}

	// End group
	ImGui::EndGroup();

	return ret;
}

bool ContentBrowser::FuzzySearch(const std::string& searchString, const std::string& directory, int threshold)
{
	std::string searchStringLower = searchString;
	std::string directoryLower = directory;

	std::transform(searchStringLower.begin(), searchStringLower.end(), searchStringLower.begin(),
		[](unsigned char c) { return std::tolower(c); });

	std::transform(directoryLower.begin(), directoryLower.end(), directoryLower.begin(),
		[](unsigned char c) { return std::tolower(c); });

	if (directoryLower.find(searchStringLower) != std::string::npos) {
		return true;
	}
	return false;
}