#include "ContentBrowser.h"

#include "imgui_internal.h"
#include "Layers/EditorLayer.h"
#include "Util.h"
#include "ThumbnailManager.h"
#include "ImGuiUtil.h"

#include <set>
#include <fstream>
#include <regex>

#include <chrono>

void ContentBrowser::OnAttach()
{
	mAssetPath = mEditorState->GetAssetsPath();
	mBrowserDirectory = mAssetPath;
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
	if (mIsOpen)
	{
		if (ImGui::Begin(Name().c_str(), &mIsOpen))
		{
			bool window_hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem | ImGuiHoveredFlags_ChildWindows);
			bool any_item_hovered = ImGui::IsAnyItemHovered();
			bool mouse_clicked = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
			if ((window_hovered && !any_item_hovered && mouse_clicked) || (!window_hovered && mouse_clicked))
			{
				mSelectedFiles.clear();
				RenameFile();
				mLastSelectedPath = fs::path();
			}
			float width = ImGui::GetContentRegionAvail().x;
			float windowPadding = ImGui::GetStyle().WindowPadding.x;
			if (ImGui::BeginChild("Folders", { width * 0.15f - 1.f * windowPadding, 0 }, ImGuiChildFlags_Border))
			{
				ImGui::Text("Assets");
				ImGui::Separator();

				for (auto& path : fs::directory_iterator(mAssetPath))
				{
					if (path.is_directory())
					{
						float width = ImGui::GetContentRegionAvail().x;
						
						if (ImGui::Button(path.path().filename().string().c_str(), { width, 0 }))
						{
							mBrowserDirectory = path.path();
							mShouldSearch = false;
							memset(mSearchBuffer, 0, 256);
							mSelectedFiles.clear();
							RenameFile();
						}

						DragDropTarget({ std::string(MULTI_FILE_PAYLOAD), SCENE_PATH_PAYLOAD }, [&](const fs::path& directory) {
							fs::path newPath = path / directory.filename();
							fs::rename(directory, newPath);
							});
						DragDropTarget(MULTI_FILE_PAYLOAD, [&](const ImGuiPayload* payload) {
							MoveSelectedFiles(path);
							});
						
					}
				}
			}
			ImGui::EndChild();
			ImGui::SameLine();
			if (ImGui::BeginChild("Content", { width * 0.85f, 0 }, ImGuiChildFlags_Border))
			{
				ImGui::Text("Last Selected: %s", mLastSelectedPath.string().c_str());
				auto assetDir = mBrowserDirectory.lexically_relative(mAssetPath.parent_path());
				ImGui::Text("Path:");
				ImGui::SameLine();

				fs::path iterativePath;
				for (auto& file : assetDir)
				{
					std::string breadCrumb = file == "." ? "Assets" : file.string();
					iterativePath /= file;
					ImGui::SameLine();

					if (ImGui::Button(breadCrumb.c_str()))
					{
						mBrowserDirectory = mAssetPath.parent_path() / iterativePath;
						mShouldSearch = false;
						memset(mSearchBuffer, 0, 256);
						mSelectedFiles.clear();
					}
					if(!mSelectedFiles.contains(mAssetPath.parent_path() / iterativePath))
						DragDropTarget(SCENE_PATH_PAYLOAD, [&](const fs::path& directory) {
						fs::path newPath = mAssetPath.parent_path() / iterativePath / directory.filename();
						fs::rename(directory, newPath);
							});
					DragDropTarget(MULTI_FILE_PAYLOAD, [&](const fs::path& directory) {
						MoveSelectedFiles(mAssetPath.parent_path() / iterativePath);
						});

					ImGui::SameLine();
					ImGui::Text("\\");
				}

				ImGui::SameLine();
				ImGui::PushItemWidth(150.f);
				if (ImGui::InputText("Search", mSearchBuffer, 256))
				{
					mSearchString = mSearchBuffer;
					if (!mSearchString.empty())
					{
						mShouldSearch = true;
					}
					else
					{
						mShouldSearch = false;
					}
				}

				ImGui::Separator();
				if (ImGui::BeginChild("Content-Data", ImGui::GetContentRegionAvail()))
				{
					if (ImGui::BeginPopupContextWindow("##RightClickContent"))
					{
						if (ImGui::BeginMenu("Create"))
						{
							if (ImGui::MenuItem("Material"))
							{
								std::string materialName = "Material.mmaterial";
								if (fs::exists(mBrowserDirectory / materialName))
									materialName = GenerateNewFilename(materialName, mBrowserDirectory);

								//auto material = Mule::Material::Create(mBrowserDirectory / materialName);
								//material->Save();
							}
							if (ImGui::MenuItem("Folder"))
							{
								std::string folderName = "New Folder";
								if (fs::exists(mBrowserDirectory / folderName))
									folderName = GenerateNewFilename(folderName, mBrowserDirectory);

								fs::create_directory(mBrowserDirectory / folderName);
							}
							ImGui::EndMenu();
						}
						ImGui::EndPopup();
					}

					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 10, 10 });
					const float maxWidth = ImGui::GetContentRegionAvail().x;
					const float padding = ImGui::GetStyle().ItemSpacing.x;
					float total = mFileWidth;

					std::vector<fs::directory_entry> paths;
					for (auto& dir : fs::directory_iterator(mBrowserDirectory))
						paths.push_back(dir);
					std::sort(paths.begin(), paths.end(), [](const fs::directory_entry& lhs, const fs::directory_entry& rhs)
						{
							return (int)lhs.is_directory() > (int)rhs.is_directory();
						});

					for (auto& dir : paths)
					{
						if (mShouldSearch)
						{
							if (!FuzzySearch(mSearchString, dir.path().filename().string(), 3))
							{
								continue;
							}
						}

						total += mFileWidth + padding;

						bool clicked = false, doubleClicked = false;
						DisplayFile(dir, clicked, doubleClicked);

						if(dir.is_directory())
						{
							DragDropTarget(SCENE_PATH_PAYLOAD, [&](const fs::path& directory) {
								fs::path newPath = dir.path() / directory.filename();
								fs::rename(directory, newPath);
								});
							DragDropTarget(MULTI_FILE_PAYLOAD, [&](const ImGuiPayload* payload) {
								MoveSelectedFiles(dir.path());
								});
							if (doubleClicked)
							{
								mBrowserDirectory = dir.path();
								mSelectedFiles.clear();
								mLastSelectedPath = fs::path();
							}
						}
						bool payloadSet = false;
						if (mSelectedFiles.size() > 1)
						{
							DragDropSource(MULTI_FILE_PAYLOAD, nullptr, 0, true, [&]() {
								float totalTemp = mFileWidth;

								for (const fs::path& path : mSelectedFiles)
								{
									totalTemp += mFileWidth + padding;
									bool a, b;
									fs::directory_entry entry;
									entry.assign(path);
									DisplayFile(entry, a, b);
									if (totalTemp >= maxWidth)
										totalTemp = mFileWidth;
									else
										ImGui::SameLine();
								}
								});
						}
						else
						{
							const char* payloadType = nullptr;
							switch (TypeFromPath(dir))
							{
							case FileType::Scene: payloadType = SCENE_PATH_PAYLOAD; break;
							default: payloadType = FILE_PAYLOAD; break;
							}
							DragDropSource(payloadType, dir.path(), [&, this]() {
								bool a, b;
								DisplayFile(dir, a, b);
								});
						}

						bool ctrl = ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl);
						bool shift = ImGui::IsKeyDown(ImGuiKey_LeftShift) || ImGui::IsKeyDown(ImGuiKey_RightShift);
						if (ImGui::IsItemHovered() && ctrl && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
						{
							if (mSelectedFiles.contains(dir.path()))
							{
								mSelectedFiles.erase(dir.path());
								mLastSelectedPath = fs::path();
							}
							else
							{
								mSelectedFiles.insert(dir.path());
								mLastSelectedPath = dir.path();
							}
						}
						else if (ImGui::IsItemHovered() && shift && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
						{
							if (mLastSelectedPath.empty()) // select all paths until this one
							{
								mSelectedFiles.clear();
								for (auto& path : paths)
								{
									mSelectedFiles.insert(path);
									if (path == dir.path())
										break;
								}
							}
							else
							{
								auto begin = std::find(paths.begin(), paths.end(), mLastSelectedPath);
								auto end = std::find(paths.begin(), paths.end(), dir.path());
								if (begin > end)
								{
									auto temp = begin;
									begin = end;
									end = temp;
								}
								for (auto it = begin; it <= end; it++)
								{
									mSelectedFiles.insert(*it);
								}

							}
						}
						else if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
						{
							bool exists = mSelectedFiles.contains(dir.path());
							mSelectedFiles.clear();
							if (!exists)
							{
								mSelectedFiles.insert(dir.path());
								mLastSelectedPath = dir.path();
							}
						}
						
						if (total >= maxWidth)
							total = mFileWidth;
						else
							ImGui::SameLine();
					}
					ImGui::PopStyleVar();
				}
				ImGui::EndChild();
			}
			ImGui::EndChild();
		}
		ImGui::End();
	}
}

static ImU32 Vec4ToImCol32(const ImVec4& color) {
	return IM_COL32(
		static_cast<int>(color.x * 255.0f),
		static_cast<int>(color.y * 255.0f),
		static_cast<int>(color.z * 255.0f),
		static_cast<int>(color.w * 255.0f)
	);
}

void ContentBrowser::MoveSelectedFiles(const fs::path& path)
{
	Mule::AssetManager& manager = Mule::AssetManager::Get();
	for (const auto& p : mSelectedFiles)
	{
		fs::path newPath = path / p.filename();

		Mule::Ref<Mule::Asset> asset = manager.GetAssetByFilepath<Mule::Asset>(path);
		if (asset)
		{
			asset->SetDirectory(path);
		}


		fs::rename(p, newPath);
	}
	mSelectedFiles.clear();
}

// TODO: Rename Asset
void ContentBrowser::RenameFile()
{
	mRenameInProgress = false;
	std::string ext = mRenamePath.extension().string();
	std::string newFilename = mRenameBuffer;

	if (!newFilename.ends_with(ext))
	{
		newFilename += ext;
	}

	const std::regex pattern("^[a-zA-Z0-9_\\.\\-\\(\\) ]+$");
	if (std::regex_match(newFilename, pattern))
	{
		fs::rename(mRenamePath, mRenamePath.parent_path() / newFilename);
		mRenamePath = fs::path();
		memset(mRenameBuffer, 0, 256);
	}
}

void ContentBrowser::DisplayFile(const fs::directory_entry& entry, bool& clicked, bool& doubleClicked)
{
	FileType type = TypeFromPath(entry);

	const float borderWidth = 2.f;
	const float height = 225.f;
	const float textPadding = 5.f;
	ImVec4 tintColor = ImVec4(0, 0, 0, 0);
	const ImVec4 selectedColor = ImVec4(0.1f, 0.1f, 0.1f, 0.3f);
	
	auto cursor = ImGui::GetCursorScreenPos();

	ImRect rect;
	rect.Min = cursor;
	rect.Max = { cursor.x + mFileWidth, cursor.y + height };
	ImGui::ItemAdd(rect, (ImGuiID)&entry.path());
	
	ImGui::BeginGroup();

	ImGui::InvisibleButton(entry.path().string().c_str(), { mFileWidth, height }, ImGuiButtonFlags_PressedOnClick | ImGuiButtonFlags_MouseButtonLeft);
	if (ImGui::IsItemHovered())
	{
		tintColor = ImVec4(0.1f, 0.1f, 0.1f, 0.2f);
	}
	if (ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left))
	{
		tintColor = ImVec4(0.1f, 0.1f, 0.1f, 0.3f);
	}
	if (mSelectedFiles.contains(entry.path()))
	{
		tintColor = ImVec4(0.1f, 0.1f, 0.9f, 0.25f);
	}

	clicked = ImGui::IsItemClicked();
	doubleClicked = ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && ImGui::IsItemHovered();

	Mule::AssetManager& manager = Mule::AssetManager::Get();
	if (ImGui::BeginPopupContextItem(entry.path().filename().string().c_str()))
	{
		mRenamePath = fs::path();
		if (ImGui::MenuItem("Rename"))
		{
			mRenameInProgress = true;
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
	
	ImTextureID id = ThumbnailManager::Get().GetThumbNail(entry.path());

	if (id != 0)
	{
		ImGui::Image(id, imageSize);
	}
	
	// Display text
	ImGui::SetCursorScreenPos({ cursor.x + textPadding + borderWidth, cursor.y + mFileWidth + textPadding });

	if (mRenamePath == entry.path())
	{
		ImGui::SetNextItemWidth(mFileWidth - 2 * (textPadding + borderWidth));
		if (ImGui::InputText("##RenameFile",
			mRenameBuffer, 256,
			ImGuiInputTextFlags_EnterReturnsTrue))
		{
			mRenameInProgress = false;
			RenameFile();			
		}
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

	// End group
	ImGui::EndGroup();
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