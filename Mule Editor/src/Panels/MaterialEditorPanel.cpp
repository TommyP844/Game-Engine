#include "MaterialEditorPanel.h"
#include "Layers/EditorLayer.h"

void MaterialEditorPanel::OnAttach()
{
}

void MaterialEditorPanel::OnUpdate(float dt)
{
	if (!mIsOpen)
	{
		mLayer->PopPanelAfterUpdate(PanelType::MaterialEditorPanel);
		return;
	}

	Mule::AssetManager& manager = Mule::AssetManager::Get();
	int flags = mModified ? ImGuiWindowFlags_UnsavedDocument : 0;
	if (ImGui::Begin(Name().c_str(), &mIsOpen, flags))
	{
		DRAG_DROP_TARGET(DRAG_DROP_MATERIAL_FILE, [&](const ImGuiPayload* payload) {
			std::string path = std::string((char*)payload->Data, payload->DataSize);
			if (manager.DoesAssetExist(path, Mule::AssetType::Material))
			{
				mMaterial = manager.GetAssetByFilepath<Mule::Material>(path);
			}
			else
			{
				fs::path p = path;
				mMaterial = Mule::Material::Load(p);
				manager.InsertAsset(mMaterial);
			}
			});
		if (mMaterial)
		{
			if (mModified)
			{
				mMaterial->SetModified();
			}
			
			if (ImGui::BeginTable("##MaterialEditValues", 2))
			{
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::Text("Name");
				ImGui::TableNextColumn();
				ImGui::Text(mMaterial->Name().c_str());

				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::Text("Tint Color");
				ImGui::TableNextColumn();
				mModified |= ImGui::ColorEdit3("##Material Color Edit", &mMaterial->AlbedoColor[0], ImGuiColorEditFlags_DisplayRGB);

				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::Text("Metalness");
				ImGui::TableNextColumn();
				mModified |= ImGui::DragFloat("##Metalness", &mMaterial->MetalnessFactor, 0.01f, 0.f, 1.f, "%.2f");
				
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::Text("Roughness");
				ImGui::TableNextColumn();
				mModified |= ImGui::DragFloat("##Roughness", &mMaterial->RoughnessFactor, 0.01f, 0.f, 1.f, "%.2f");

				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::Text("Ambient Occlusion");
				ImGui::TableNextColumn();
				mModified |= ImGui::DragFloat("##Ambient Occlusion", &mMaterial->AmbientOcclusionFactor, 0.01f, 0.f, 1.f, "%.2f");

				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::Text("Has Transparency");
				ImGui::TableNextColumn();
				mModified |= ImGui::Checkbox("##Has Transparency", &mMaterial->HasTransparency);

				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::Text("Alpha");
				ImGui::TableNextColumn();
				ImGui::BeginDisabled(!mMaterial->HasTransparency);
				mModified |= ImGui::DragFloat("##Alpha", &mMaterial->Transparency, 0.001f, 0.f, 1.f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
				ImGui::EndDisabled();

				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::Text("Tex Coord Scale");
				ImGui::TableNextColumn();
				mModified |= ImGui::DragFloat2("##Tex Coord Scale", &mMaterial->TextureScale[0], 0.001f, 0.001f, 1000.f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
				
				ImGui::EndTable();
			}
			ImGui::Separator();

			DisplayTexture("Albedo Map", mMaterial->AlbedoMap);
			ImGui::Separator();
			DisplayTexture("Normal Map", mMaterial->NormalMap);
			ImGui::Separator();
			DisplayTexture("Roughness Map", mMaterial->RoughnessMap);
			ImGui::Separator();
			DisplayTexture("Metalness Map", mMaterial->MetalnessMap);
			ImGui::Separator();
			DisplayTexture("Ambient Occlusion Map", mMaterial->AmbientOcclusionMap);
		}
	}
	ImGui::End();
}

void MaterialEditorPanel::OnDetach()
{
}

void MaterialEditorPanel::OnEvent(Mule::Ref<Mule::Event> event)
{
	switch (event->Type())
	{
	case Mule::EventType::WindowEvent:
		{
		Mule::WeakRef<Mule::WindowEvent> windowEvent = event;
		if (windowEvent->KeyPressed[(int)Mule::Key::Key_S]
			&& (windowEvent->KeyPressed[(int)Mule::Key::Key_LeftControl]
				|| windowEvent->KeyPressed[(int)Mule::Key::Key_RightControl]))
		{
			mMaterial->Save();
			mModified = false;
		}
		}
		break;
	}
}

void MaterialEditorPanel::DisplayTexture(const char* name, Mule::AssetHandle& handle)
{
	Mule::AssetManager& manager = Mule::AssetManager::Get();
	Mule::Ref<Mule::Texture> texture = manager.GetAsset<Mule::Texture>(handle);
	
	ImTextureID id = Mule::Texture::Default2D()->ImGuiID();
	const char* texName = "(Empty)";
	int width = 0;
	int height = 0;

	if (texture)
	{
		width = texture->Width();
		height = texture->Height();
		texName = texture->Name().c_str();
		id = texture->ImGuiID();
	}	

	ImGui::Image(id, {200.f, 200.f});
	DRAG_DROP_TARGET(DRAG_DROP_TEXTURE_FILE, [&](const ImGuiPayload* payload) {
		std::string path = std::string((char*)payload->Data, payload->DataSize);
		if (manager.DoesAssetExist(path, Mule::AssetType::Texture))
		{
			auto textureHandle = manager.GetAssetByFilepath<Mule::Texture>(path)->Handle();
			handle = textureHandle;
			mModified = true;
		}
		else
		{
			Mule::Ref<Mule::Texture> tex = Mule::Texture::Load2D(path);
			manager.InsertAsset(tex);
			handle = tex->Handle();
			mModified = true;
		}
		});
	if (ImGui::BeginPopupContextItem((std::string("##MaterialTexture-") + name).c_str()))
	{
		if (ImGui::MenuItem("Remove"))
		{
			handle = Mule::Asset::NullHandle;
			mModified = true;
		}
		ImGui::EndPopup();
	}

	ImGui::SameLine();
	ImGui::BeginGroup();

	ImGui::Text("Type: %s", name);
	ImGui::Text("Name: %s", texName);
	ImGui::Text("Size: %ix%i", width, height);
	ImGui::Text("Handle: %zu", handle);

	ImGui::EndGroup();
}
