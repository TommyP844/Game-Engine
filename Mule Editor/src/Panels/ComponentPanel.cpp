#include "ComponentPanel.h"
#include "ImGuiUtil.h"
#include "ComponentDisplay.h"

static void ScriptFieldEdit(Mule::Ref<Mule::ScriptableClass> scriptableClass, Mule::Ref<Mule::Scene> scene)
{
	Mule::MonoEngine& mono = Mule::MonoEngine::Get();

	for (const auto& [name, type] : scriptableClass->Fields)
	{
		std::string lambdaId = "##" + name + ":script";
		const char* cLambdaId = lambdaId.c_str();
		switch (type)
		{
		case Mule::CSType::Int:
			ValueEdit(name, [&]() {
				int val = scriptableClass->GetFieldValue<int>(name);
				if (ImGui::DragInt(cLambdaId, &val))
				{
					scriptableClass->SetFieldValue(name, val);
				}
				return false;
				});
			break;
		case Mule::CSType::Boolean:
			ValueEdit(name, [&]() {
				bool val = scriptableClass->GetFieldValue<bool>(name);
				if (ImGui::Checkbox(cLambdaId, &val))
				{
					scriptableClass->SetFieldValue(name, val);
				}
				return false;
				});
			break;
		case Mule::CSType::Entity:
			ValueEdit(name, [&]() {
				Mule::Ref<Mule::ScriptableClass> scriptable = scriptableClass->GetFieldValue(name);
				std::string entityName = "null";
				if (scriptable)
				{
					int id = scriptable->GetFieldValue<int>("id");
					Mule::Entity e((entt::entity)id, scene);
					entityName = e.Tag();
				}
				ImGui::Text("Entity (%s)", entityName.c_str());
				DRAG_DROP_TARGET(DRAG_DROP_ENTITY, [&](const ImGuiPayload* payload) {
					Mule::Entity payloadEntity(*(entt::entity*)payload->Data, scene);
					Mule::WeakRef<Mule::Entity> entityRef = &payloadEntity;
					scriptableClass->SetFieldValue(name, entityRef);
					});
				return false;
				});
		break;
		default:
			ImGui::Text("Not implemented");
			break;
		}
	}
}

void ComponentPanel::OnAttach()
{
}

void ComponentPanel::OnUpdate(float dt)
{
	if (ImGui::Begin(Name().c_str()))
	{
		if (mEditorState.SelectedEntities.size() == 1)
		{
			Mule::Entity entity = *(mEditorState.SelectedEntities.begin());
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
					entity.GetComponent<Mule::TagComponent>().Tag = buffer;
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

				Mule::Transform& transform = entity.GetComponent<Mule::TransformComponent>().Transform;

				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::Text("Translation");
				ImGui::TableNextColumn();
				Vec3Display("Translation", transform.Translation);

				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::Text("Orientation");
				ImGui::TableNextColumn();
				Vec3DisplayDegrees("Orientation", transform.Orientation);

				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::Text("Scale");
				ImGui::TableNextColumn();
				Vec3Display("Scale", transform.Scale, {1.f, 1.f, 1.f});

				ImGui::EndTable();
			}
			ImGui::Separator();
			
			if(ImGui::BeginMenu("Add Component"))
			{
				if (ImGui::MenuItem("Camera")) entity.AddComponent<Mule::CameraComponent>();
				if (ImGui::MenuItem("Directional Light")) entity.AddComponent<Mule::DirectionalLightComponent>();
				if (ImGui::MenuItem("Model")) entity.AddComponent<Mule::ModelComponent>();
				if (ImGui::MenuItem("Point Light")) entity.AddComponent<Mule::PointLightComponent>();
				if (ImGui::MenuItem("Script")) entity.AddComponent<Mule::ScriptComponent>();
				if (ImGui::MenuItem("Spot Light")) entity.AddComponent<Mule::SpotLightComponent>();
				ImGui::EndMenu();
			}

			bool modified = false;
			modified |= ComponentEdit<Mule::CameraComponent>(entity, "Camera", DisplayCameraComponent);
			modified |= ComponentEdit<Mule::DirectionalLightComponent>(entity, "Directional Light", DisplayDirectionalLightComponent);
			modified |= ComponentEdit<Mule::ModelComponent>(entity, "Model", DisplayModelComponent);
			modified |= ComponentEdit<Mule::PointLightComponent>(entity, "Point Light", DisplayPointLightComponent);
			modified |= ComponentEdit<Mule::ScriptComponent>(entity, "Script", DisplayScriptComponent);
			modified |= ComponentEdit<Mule::SpotLightComponent>(entity, "Spot Light", DisplaySpotLightComponent);

			if (modified) mEditorState.GetActiveScene()->SetModified();
		}
	}
	ImGui::End();
}

void ComponentPanel::OnDetach()
{
}

void ComponentPanel::OnEvent(Mule::Ref<Mule::Event> event)
{
}
