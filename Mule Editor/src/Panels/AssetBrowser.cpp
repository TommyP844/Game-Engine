#include "AssetBrowser.h"
#include "Layers/EditorLayer.h"

#include "Mule.h"

void AssetBrowser::OnAttach()
{
}

void AssetBrowser::OnUpdate(float dt)
{
	if (!mIsOpen)
	{
		mLayer->PopPanelAfterUpdate(PanelType::AssetBrowserPanel);
		return;
	}

	if (ImGui::Begin(Name().c_str(), &mIsOpen))
	{
		Mule::AssetManager& manager = Mule::AssetManager::Get();

		if (ImGui::BeginTabBar("Asset Types"))
		{
			if (ImGui::BeginTabItem("Scenes"))
			{
				auto scenes = manager.GetAllAssetsOfType(Mule::AssetType::Scene);
				for (auto& scene : scenes)
				{
					ImGui::Text("Handle: %zu", scene->Handle());
					ImGui::Text("Name: %s", scene->Name().c_str());
					ImGui::Text("Directory: %s", scene->Directory().string().c_str());
					ImGui::Separator();
				}
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Textures"))
			{
				auto assets = manager.GetAllAssetsOfType(Mule::AssetType::Texture);
				for (auto& asset : assets)
				{
					Mule::Ref<Mule::Texture> texture = asset;
					ImGui::Text("Handle: %zu", texture->Handle());
					ImGui::Text("Name: %s", texture->Name().c_str());
					ImGui::Text("Directory: %s", texture->Directory().string().c_str());
					ImGui::Separator();
				}
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Shaders"))
			{
				auto assets = manager.GetAllAssetsOfType(Mule::AssetType::Shader);
				for (auto& asset : assets)
				{
					Mule::Ref<Mule::GraphicsShader> shader = asset;
					ImGui::Text("Handle: %zu", shader->Handle());
					ImGui::Text("Name: %s", shader->Name().c_str());
					ImGui::Text("Directory: %s", shader->Directory().string().c_str());
					if (ImGui::Button("Reload"))
					{
						shader->Reload();
					}
					ImGui::Separator();
				}
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
	}
	ImGui::End();
}

void AssetBrowser::OnDetach()
{
}

void AssetBrowser::OnEvent(Mule::Ref<Mule::Event> event)
{
}
