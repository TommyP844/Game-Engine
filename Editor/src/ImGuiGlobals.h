#pragma once

#include "imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"

#define PAYLOAD_TEXTURE  "TEXTURE"
#define PAYLOAD_SHADER   "SHADER"
#define PAYLOAD_MATERIAL "MATERIAL"
#define PAYLOAD_MESH     "MESH"
#define PAYLOAD_SCENE     "SCENE"


static std::string GetVisibleID(const char* label, int number)
{
	return label + std::string("##") + std::to_string(number);
}

static std::string GetInvisbleID(const char* label, int number)
{
	return std::string("##") + label + std::to_string(number);
}

namespace ImGui
{
	static bool ImageButtonWithText(ImTextureID texId, const char* label, const ImVec2& imageSize, const ImVec2& uv0 = {0, 0}, const ImVec2& uv1 = { 1, 1 }, int frame_padding = -1, const ImVec4& bg_col = { 0, 0, 0, 0 }, const ImVec4& tint_col = { 1, 1, 1, 1 });

	bool ImageButtonWithText(ImTextureID texId, const char* label, const ImVec2& imageSize, const ImVec2& uv0, const ImVec2& uv1, int frame_padding, const ImVec4& bg_col, const ImVec4& tint_col)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImVec2 size = imageSize;
		if (size.x <= 0 && size.y <= 0) { size.x = size.y = ImGui::GetTextLineHeightWithSpacing(); }
		else {
			if (size.x <= 0)          size.x = size.y;
			else if (size.y <= 0)     size.y = size.x;
			size *= window->FontWindowScale * ImGui::GetIO().FontGlobalScale;
		}

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;

		const ImGuiID id = window->GetID(label);
		const ImVec2 textSize = ImGui::CalcTextSize(label, NULL, true);
		const bool hasText = textSize.x > 0;

		const float innerSpacing = hasText ? ((frame_padding >= 0) ? (float)frame_padding : (style.ItemInnerSpacing.x)) : 0.f;
		const ImVec2 padding = (frame_padding >= 0) ? ImVec2((float)frame_padding, (float)frame_padding) : style.FramePadding;
		const ImVec2 totalSizeWithoutPadding(size.x, size.y +textSize.y + padding.y);
		const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + totalSizeWithoutPadding + padding * 2);
		ImVec2 start(0, 0);
		start = window->DC.CursorPos + padding; if (size.y < textSize.y) start.y += (textSize.y - size.y) * .5f;
		const ImRect image_bb(start, start + size);
		start = window->DC.CursorPos + padding; start.y += innerSpacing + size.y;
		ItemSize(bb);
		if (!ItemAdd(bb, id))
			return false;

		bool hovered = false, held = false;
		bool pressed = ButtonBehavior(bb, id, &hovered, &held);

		// Render
		const ImU32 col = GetColorU32((hovered && held) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
		RenderFrame(bb.Min, bb.Max, col, true, ImClamp((float)ImMin(padding.x, padding.y), 0.0f, style.FrameRounding));
		if (bg_col.w > 0.0f)
			window->DrawList->AddRectFilled(image_bb.Min, image_bb.Max, GetColorU32(bg_col));

		window->DrawList->AddImage(texId, image_bb.Min, image_bb.Max, uv0, uv1, GetColorU32(tint_col));

		// if (textSize.x > 0) ImGui::RenderText(start, label);
		//if (textSize.x > 0) ImGui::RenderTextClipped(start, {start.x + size.x, start.y + ImGui::GetTextLineHeight()}, label, NULL, NULL);
		ImGui::RenderTextEllipsis(window->DrawList, start, { start.x + size.x, start.y + ImGui::GetTextLineHeight() + padding.y },
			start.x + size.x,
			start.x + size.x,
			label,
			NULL,
			NULL);
		
		return pressed;
	}

	static bool SearchCombo(const char* label, char* buffer, int bufSize, const std::vector<std::string>& options, bool clearfilter = false)
	{
		std::string hiddenInputLabel = "##input" + std::string(label);
		std::string hiddenComboLabel = "Material##combo" + std::string(label);
		bool ret = false;
		if (ImGui::InputText(hiddenInputLabel.c_str(), buffer, bufSize, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			ret = true;
		}

		if (ImGui::IsItemFocused())
			ImGui::SetNextItemOpen(true);

		ImGui::SameLine();
		const char* preview = "prev";
		
		
		if (ImGui::BeginCombo(hiddenComboLabel.c_str(), preview, ImGuiComboFlags_NoPreview))
		{
			bool selected = false;
			for (int i = 0; i < options.size(); i++)
			{
				if (ImGui::Selectable(options[i].c_str(), &selected))
				{
					memcpy(buffer, options[i].c_str(), options[i].size());
					ret = true;
				}
			}
			ImGui::EndCombo();
		}

		return ret;
	}
}

static bool NewFileExplorer(AppData& appData, const std::string& label, char* filename, int filenameLen, char* dir, int dirLen)
{
	bool ret = false;
	if (ImGui::BeginPopupModal(label.c_str()))
	{
		static fs::path currentPath = appData.AssetDir;
		ImGui::Text(currentPath.u8string().c_str());

		auto newMaterialPopupSize = ImGui::GetWindowSize();
		if (ImGui::BeginChild((label + "child").c_str(), { newMaterialPopupSize.x, newMaterialPopupSize.y * 0.8f }))
		{
			for (auto path : fs::directory_iterator(currentPath))
			{
				ImVec2 pos = ImGui::GetCursorPos();

				ImVec2 size = ImGui::GetContentRegionAvail();
				bool sameLine = true;
				if (pos.x + 240 > pos.x + size.x)
					sameLine = false;
				if (path.is_directory())
				{
					if (ImGui::ImageButtonWithText(appData.EditorTextures["FolderIcon"]->GetImguiID(), path.path().filename().u8string().c_str(), { 120, 120 }))
					{
						currentPath = path;
						std::string strPath = currentPath.u8string();
						memcpy(dir, strPath.data(), strPath.size()); // for some reason this populates filename buffer
					}
				}
				if (sameLine) ImGui::SameLine();
			}
			ImGui::EndChild();
		}
		if (ImGui::Button("Back"))
		{
			if (currentPath != appData.AssetDir)
			{
				currentPath = currentPath.parent_path();
				std::string strPath = currentPath.u8string();
				memset(dir, 0, dirLen);
				memcpy(dir, strPath.data(), strPath.size());
			}
		}

		if (ImGui::InputText("Name", filename, filenameLen, ImGuiInputTextFlags_EnterReturnsTrue))
		{

			ret = true;
			ImGui::CloseCurrentPopup();
		}
		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Create"))
		{
			ret = true;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
	return ret;
}
