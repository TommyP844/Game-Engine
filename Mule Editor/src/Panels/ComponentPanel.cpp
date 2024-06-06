#include "ComponentPanel.h"
#include "ImGuiUtil.h"
#include "ComponentDisplay.h"

void ComponentPanel::OnAttach()
{
}

void ComponentPanel::OnUpdate(float dt)
{
}

void ComponentPanel::OnDetach()
{
}

void ComponentPanel::OnEvent(Mule::Ref<Mule::Event> event)
{
}

void ComponentPanel::OnImGuiRender()
{
	if (mIsOpen)
	{
		int flags = 0;
		
		auto scene = mEditorState->GetActiveScene();
		if (scene)
		{
			flags |= scene->Modified() ? ImGuiWindowFlags_UnsavedDocument : 0;
		}

		if (ImGui::Begin(Name().c_str(), &mIsOpen, flags))
		{
			if (mEditorState->SelectedEntities.size() == 1)
			{
				bool modified = false;
				Mule::Entity entity = *(mEditorState->SelectedEntities.begin());
				float tableWidth = ImGui::GetContentRegionAvail().x;
				if (ImGui::BeginTable("Entity Info", 2, 0, { 0, 0 }, tableWidth))
				{

					ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 100.f);
					ImGui::TableSetupColumn("Translation");

					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					ImGui::Text("Tag");
					ImGui::TableNextColumn();
					char buffer[256] = { 0 };
					memcpy(buffer, entity.Tag().c_str(), entity.Tag().size());
					ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
					if (ImGui::InputText("##Tag", buffer, 256))
					{
						entity.GetComponent<Mule::MetaComponent>().Tag = buffer;
						modified = true;
					}

					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					ImGui::Text("ID ");

					ImGui::TableNextColumn();
					ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
					std::string idStr = std::to_string((int)entity.ID());
					ImGui::InputText("##guid", idStr.data(), idStr.size(), ImGuiInputTextFlags_ReadOnly);

					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					ImGui::Text("Guid ");
					std::string guidStr = std::to_string((size_t)entity.Guid());

					ImGui::TableNextColumn();
					ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
					ImGui::InputText("##guid", guidStr.data(), guidStr.size(), ImGuiInputTextFlags_ReadOnly);

					Mule::Transform& transform = entity.GetComponent<Mule::MetaComponent>().Transform;

					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					ImGui::Text("Translation");
					ImGui::TableNextColumn();
					modified |= Vec3Display("Translation", transform.Translation);

					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					ImGui::Text("Orientation");
					ImGui::TableNextColumn();
					modified |= Vec3Display("Orientation", transform.Rotation);

					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					ImGui::Text("Scale");
					ImGui::TableNextColumn();
					modified |= Vec3Display("Scale", transform.Scale, { 1.f, 1.f, 1.f });

					ImGui::EndTable();
				}
				ImGui::Separator();

				if (ImGui::BeginMenu("Add Component"))
				{
					if (ImGui::MenuItem("Camera")) entity.AddComponent<Mule::CameraComponent>();
					if (ImGui::MenuItem("Directional Light")) entity.AddComponent<Mule::DirectionalLightComponent>();
					if (ImGui::MenuItem("Mesh"))
					{
						if (!entity.HasComponent<Mule::MeshCollectionComponent>())
							entity.AddComponent<Mule::MeshCollectionComponent>();
						
						auto& collection = entity.GetComponent<Mule::MeshCollectionComponent>();
						collection.Meshes.push_back(Mule::MeshComponent());
						
					}
					if (ImGui::MenuItem("Point Light")) entity.AddComponent<Mule::PointLightComponent>();
					if (ImGui::MenuItem("Script")) entity.AddComponent<Mule::ScriptComponent>();
					if (ImGui::MenuItem("Spot Light")) entity.AddComponent<Mule::SpotLightComponent>();
					ImGui::EndMenu();
				}

				modified |= ComponentEdit<Mule::CameraComponent>(entity, "Camera", DisplayCameraComponent);
				modified |= ComponentEdit<Mule::DirectionalLightComponent>(entity, "Directional Light", DisplayDirectionalLightComponent);
				if (entity.HasComponent<Mule::MeshCollectionComponent>())
				{
					auto& collection = entity.GetComponent<Mule::MeshCollectionComponent>();
					int eraseIndex = -1;
					for (int i = 0; i < collection.Meshes.size(); i++)
					{
						auto& meshComponent = collection.Meshes[i];
						bool remove = false;
						std::string hash = "##" + std::to_string(meshComponent.GetGuid());
						modified |= CollectionEdit<Mule::MeshComponent>(entity, "Mesh" + hash, meshComponent, remove, DisplayMeshComponent);
						if (remove)
						{
							modified = true;
							eraseIndex = i;
						}
					}
					if (eraseIndex >= 0)
					{
						collection.Meshes.erase(collection.Meshes.begin() + eraseIndex);
					}
				}
				modified |= ComponentEdit<Mule::PointLightComponent>(entity, "Point Light", DisplayPointLightComponent);
				modified |= ComponentEdit<Mule::ScriptComponent>(entity, "Script", DisplayScriptComponent);
				modified |= ComponentEdit<Mule::SpotLightComponent>(entity, "Spot Light", DisplaySpotLightComponent);

				if (modified) mEditorState->GetActiveScene()->SetModified();
			}
		}
		ImGui::End();
	}
}
