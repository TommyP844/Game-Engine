#include "NewAssetPopup.h"

#include "Util.h"

NewAssetPopUp::NewAssetPopUp(const std::string& name, const fs::path& assetDirectory, const fs::path& defaultDirectory, const std::string assetExtension)
	:
	mName(name),
	mAssetDir(assetDirectory),
	mDirectory(defaultDirectory),
	mExtension(assetExtension),
	mShowPopup(true)
{
}

Mule::Ref<NewAssetPopUp> NewAssetPopUp::Create(const std::string& name, const fs::path& assetDirectory, const fs::path& defaultDirectory, const std::string assetExtension)
{
	return Mule::Ref<NewAssetPopUp>(new NewAssetPopUp(name, assetDirectory, defaultDirectory, assetExtension));
}

bool NewAssetPopUp::OnImGuiRender(fs::path& path)
{
	bool ret = false;

	if (mShowPopup)
	{
		memset(mInputBuffer, 0, 256);
		mShowPopup = false;
		ImGui::OpenPopup(mName.c_str());
	}

	int popupFlags = ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoResize;

	if (ImGui::BeginPopup(mName.c_str(), popupFlags))
	{
		std::string filename = mInputBuffer;
		if (!filename.ends_with(mExtension))
			filename += "." + mExtension;
		fs::path file = mDirectory / filename;

		ImGui::Text(mName.c_str());
		fs::path directory = mDirectory.lexically_relative(mAssetDir);
		directory = "Assets" / directory / "";
		ImGui::Text("Directory: %s", directory.string().c_str());
		ImGui::SameLine(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize("...").x);
		if (ImGui::Button("..."))
		{
			const char* selectedPath = SelectFolderDialog(
				"Directory",
				mDirectory.string().c_str()
			);

			if (selectedPath != NULL)
			{
				if (!fs::path(selectedPath).lexically_relative(mAssetDir).string().starts_with(".."))
					mDirectory = selectedPath;
			}
		}
		ImGui::Separator();
		ImGui::Text("Name");
		ImGui::SameLine();
		ImGui::PushItemWidth(250.f);
		bool invalid = (!IsFilenameValid(mInputBuffer) || fs::exists(file)) && strlen(mInputBuffer) > 0;
		if (invalid) { ImGui::PushStyleColor(ImGuiCol_Text, { 1, 0, 0.05, 1 }); }
		ImGui::InputText(("##" + mName).c_str(), mInputBuffer, 256);
		if (invalid) { ImGui::PopStyleColor(); }

		// TODO: Add filesystem explorer
		ImGui::BeginDisabled(invalid);
		if (ImGui::Button("Create"))
		{
			ret = true;
			path = file;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndDisabled();

		float buttonWidth = ImGui::CalcTextSize("Close").x;

		ImGui::SameLine(ImGui::GetContentRegionAvail().x - buttonWidth);

		if (ImGui::Button("Close"))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();

		return ret;
	}
}
