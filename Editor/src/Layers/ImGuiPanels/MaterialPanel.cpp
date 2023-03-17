#include "MaterialPanel.h"

#include "imgui.h"
#include "../../ImGuiGlobals.h"
#include "Mule/ECS/Serializer.h"

int MaterialPanel::sWindowID = 0;

MaterialPanel::MaterialPanel(AppData& appData)
	: 
	IPanel("Material Panel", appData)
{
	mWindowID = sWindowID++;
}

MaterialPanel::MaterialPanel(AppData& appData, Mule::Ref<Mule::Material> openMaterial)
	: 
	IPanel("Material Panel", appData),
	mEditMaterial(openMaterial)
{
	mWindowID = sWindowID++;
}

MaterialPanel::MaterialPanel(AppData& appData, const fs::path& materialPath)
	:
	IPanel("Material Panel", appData)
{
	mWindowID = sWindowID++;
	mEditMaterial = Mule::MakeRef<Mule::Material>();
	std::string name = materialPath.filename().u8string();
	name = name.substr(0, name.find_last_of("."));
	mEditMaterial->Name = name;
	Mule::AssetManager& manager = Mule::AssetManager::Get();
	manager.TrackAsset(name, mEditMaterial, materialPath, Mule::Asset_Material);
	Mule::SerializeMaterial(materialPath, mEditMaterial);
}

MaterialPanel::~MaterialPanel()
{
	sWindowID--;
}

void MaterialPanel::OnAttach()
{
}

void MaterialPanel::OnUpdate()
{
	std::string windowName = "Material Editor";
	if (mWindowID > 0)
	{
		windowName += " (" + std::to_string(mWindowID) + ")";
	}
	if (ImGui::Begin(windowName.c_str()))
	{
		Mule::AssetManager& manager = Mule::AssetManager::Get();
		// searchbar / combo box for finding materials
		auto names = manager.GetAssetNamesOfType(Mule::Asset_Material);
		std::vector<std::string> options;
		if (strlen(mBuffer) > 0)
		{
			for (auto& name : names)
			{
				if (name.find(mBuffer) != std::string::npos)
				{
					options.push_back(name);
				}
			}
		}
		else
		{
			options = names;
		}

		std::string SearchName = "SearchMaterial" + std::to_string(mWindowID);
		if (ImGui::SearchCombo(SearchName.c_str(), mBuffer, 64, options, false))
		{
			mEditMaterial = manager.GetAsset<Mule::Material>(mBuffer);
		}

		ImGui::Separator();

		// current editable material
		if (mEditMaterial)
		{
			bool modified = false;
			
			ImGui::Text("Material Name: %s", mEditMaterial->Name.c_str());
			
			modified |= ImGui::ColorEdit4(GetVisibleID("Albedo Color", mWindowID).c_str(), &mEditMaterial->AlbedoColor[0], ImGuiColorEditFlags_Float);
			modified |= ImGui::DragFloat(GetVisibleID("Transparency", mWindowID).c_str(), &mEditMaterial->Transparency, 0.01f, 0.f, 1.f);
			modified |= ImGui::DragFloat(GetVisibleID("Metalness", mWindowID).c_str(), &mEditMaterial->Metalness, 0.01f, 0.f, 1.f);
			modified |= ImGui::DragFloat(GetVisibleID("Roughness", mWindowID).c_str(), &mEditMaterial->Roughnes, 0.01f, 0.f, 1.f);
			modified |= ImGui::DragFloat2(GetVisibleID("Scale", mWindowID).c_str(), &mEditMaterial->MaterialScale[0], 0.1f, 0.1f, 100.f);


			ImGui::Separator();
			Mule::Ref<Mule::Texture> albedoMap = manager.GetAsset<Mule::Texture>(mEditMaterial->AlbedoMap);
			modified |= DisplayMaterialTexture(mEditMaterial->AlbedoMap, albedoMap, "Albedo Map", MaterialTextureSlot_Albedo);
			Mule::Ref<Mule::Texture> normalMap = manager.GetAsset<Mule::Texture>(mEditMaterial->NormalMap);
			modified |= DisplayMaterialTexture(mEditMaterial->NormalMap, normalMap, "Normal Map", MaterialTextureSlot_Normal);
			Mule::Ref<Mule::Texture> roughnessMap = manager.GetAsset<Mule::Texture>(mEditMaterial->RoughnessMap);
			modified |= DisplayMaterialTexture(mEditMaterial->RoughnessMap, roughnessMap, "Roughness Map", MaterialTextureSlot_Roughness);
			Mule::Ref<Mule::Texture> metalicMap = manager.GetAsset<Mule::Texture>(mEditMaterial->MetalnessMap);
			modified |= DisplayMaterialTexture(mEditMaterial->MetalnessMap, metalicMap, "Metalness Map", MaterialTextureSlot_Metalness);
			Mule::Ref<Mule::Texture> aoMap = manager.GetAsset<Mule::Texture>(mEditMaterial->AOMap);
			modified |= DisplayMaterialTexture(mEditMaterial->AOMap, aoMap, "AO Map", MaterialTextureSlot_AO);

			if (modified)
			{
				LOG_MSG(L"Material updated and saved {0}", mEditMaterial->Name);
				auto materialMetaData = manager.GetAssetWithMetaData(mEditMaterial->Name);
				Mule::SerializeMaterial(materialMetaData.Path, mEditMaterial);
			}
		}
	}
	 ImGui::End();
}

void MaterialPanel::OnDetach()
{
}

bool MaterialPanel::DisplayMaterialTexture(std::string& texName, Mule::Ref<Mule::Texture> tex, const char* label, MaterialTextureSlot slot)
{
	bool modified = false;
	ImTextureID id = tex ? tex->GetImguiID() : mAppData.EditorTextures["MissingImage"]->GetImguiID();
	ImGui::Image(id, { 200, 200 });

	std::string popupID = "MaterialTextureOptions" + std::string(label);
	if (ImGui::BeginPopupContextItem(popupID.c_str()))
	{
		if (ImGui::MenuItem("Remove", "", "", !texName.empty()))
		{
			switch (slot)
			{
			case MaterialTextureSlot_Albedo: mEditMaterial->AlbedoMap = std::string(); break;
			case MaterialTextureSlot_Normal: mEditMaterial->NormalMap = std::string(); break;
			case MaterialTextureSlot_Roughness: mEditMaterial->RoughnessMap = std::string(); break;
			case MaterialTextureSlot_Metalness: mEditMaterial->MetalnessMap = std::string(); break;
			case MaterialTextureSlot_AO: mEditMaterial->AOMap = std::string(); break;
			default: break;
			}
			modified = true;
		}
		ImGui::EndPopup();
	}

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(PAYLOAD_TEXTURE))
		{
			texName = std::string((char*)payload->Data, payload->DataSize);	
			modified = true;
		}
		ImGui::EndDragDropTarget();
	}

	ImGui::SameLine();
	if (ImGui::BeginChild(GetVisibleID(label, mWindowID).c_str(), {200, 200}, false, ImGuiWindowFlags_NoScrollbar))
	{
		ImGui::Text(label);
		if (tex)
		{
			ImGui::TextWrapped("Texture: %s, %ix%i", texName.c_str(), tex->GetWidth(), tex->GetHeight());
		}
	}
	ImGui::EndChild();
	ImGui::Separator();
	return modified;
}
