#include "AssetManagerPanel.h"

#include <filesystem>




namespace fs = std::filesystem;

AssetManagerPanel::AssetManagerPanel(AppData& appData, const std::string& baseDir)
	: 
	IPanel("AssetManager", appData),
	mBaseDir(baseDir)
{
}

AssetManagerPanel::~AssetManagerPanel()
{
}

void AssetManagerPanel::OnAttach()
{
}

void AssetManagerPanel::OnUpdate()
{
	if (ImGui::Begin("Asset Manager Panel"))
	{
		if (ImGui::BeginTabBar("Asset"))
		{
			if (ImGui::BeginTabItem("Materials"))
			{
				ListAssets(Mule::Asset_Material);
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Meshes"))
			{
				ListAssets(Mule::Asset_MeshCollection);
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Textures"))
			{
				ListAssets(Mule::Asset_Texture);
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Shaders"))
			{
				ListAssets(Mule::Asset_Shader);
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Scenes"))
			{
				ListAssets(Mule::Asset_Scene);
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
	}
	ImGui::End();
}

void AssetManagerPanel::OnDetach()
{
}

void AssetManagerPanel::ListAssets(Mule::AssetType type)
{
	Mule::AssetManager& manager = Mule::AssetManager::Get();
	std::vector<Mule::AssetManager::AssetEntry> assets;
	switch (type)
	{
	case Mule::Asset_Texture:
		assets = manager.GetAssetsOfType<Mule::Texture>(Mule::Asset_Texture);
		break;
	case Mule::Asset_Shader:
		assets = manager.GetAssetsOfType<Mule::Texture>(Mule::Asset_Shader);
		break;
	case Mule::Asset_Scene:
		assets = manager.GetAssetsOfType<Mule::Texture>(Mule::Asset_Scene);
		break;
	case Mule::Asset_MeshCollection:
		assets = manager.GetAssetsOfType<Mule::Texture>(Mule::Asset_MeshCollection);
		break;
	case Mule::Asset_Material:
		assets = manager.GetAssetsOfType<Mule::Material>(Mule::Asset_Material);
		break;
	}

	for (auto& it : assets)
	{
		switch (type)
		{
		case Mule::Asset_Texture:
		{
			auto originalCursor = ImGui::GetCursorPosX();
			Mule::Ref<Mule::Texture> tex = std::static_pointer_cast<Mule::Texture>(it.Asset);
			ImGui::Image(tex->GetImguiID(), { 200, 200 });
			ImGui::SameLine();
			auto cursor = ImGui::GetCursorPos();
			float newPos = cursor.y + 100.f - ImGui::GetTextLineHeight() / 2.f;
			ImGui::SetCursorPos({ cursor.x, newPos });
			ImGui::Text(it.Name.c_str());
			float newX = ImGui::GetCursorPosX();
			ImGuiStyle& style = ImGui::GetStyle();
			ImGui::SetCursorPos({ originalCursor, cursor.y + 200 + style.FramePadding.y });
		}
			break;
		case Mule::Asset_Material:
			ImGui::Text(it.Name.c_str());
			break;
		default:
			ImGui::Text(it.Name.c_str());
			break;
		}
		
	}
}
