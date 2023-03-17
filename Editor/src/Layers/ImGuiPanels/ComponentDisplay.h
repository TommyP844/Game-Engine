#pragma once

#include "Mule/Mule.h"
#include "imgui.h"
#include "Mule/Renderer/MeshCollection.h"
#include "../../ImGuiGlobals.h"

template<typename T, typename Func>
static void DisplayComponent(const char* label, Mule::Entity entity, Func func)
{
	if (entity.HasComponent<T>())
	{
		bool shouldRemove = false;

		bool open = ImGui::TreeNode(label);

		if (ImGui::BeginPopupContextWindow("ComponentPopup"))
		{
			if (ImGui::MenuItem("Remove")) shouldRemove = true;
			ImGui::EndPopup();
		}
		
		if(open)
		{
			T& component = entity.GetComponent<T>();
			func(component, entity);
			ImGui::TreePop();
		}

		if (shouldRemove) entity.RemoveComponent<T>();
	}
}


static void DisplayMeshCollection(Mule::MeshCollectionComponent& mesh, Mule::Entity entity)
{
	static char buffer[64] = "spider";
	bool modified = false;

	if (ImGui::InputText("Search", buffer, 64))
	{
		Mule::AssetManager& manager = Mule::AssetManager::Get();
		if (manager.IsAssetTracked(buffer))
		{
			mesh.Meshes = manager.GetAsset<Mule::MeshCollection>(buffer);
			memset(buffer, 0, 64);
			modified |= true;
		}
	}
	modified |= ImGui::Checkbox("Visable", &mesh.Visible);
	ImGui::Text("Mesh: %s", mesh.Meshes ? mesh.Meshes->Name.c_str() : "No Mesh");

	const char* matName = "No Material";
	if (mesh.Material)
	{
		matName = mesh.Material->Name.c_str();
	}
	ImGui::Text("Material: %s", matName);
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(PAYLOAD_MATERIAL))
		{
			Mule::AssetManager& manager = Mule::AssetManager::Get();
			std::string name = std::string((char*)payload->Data, payload->DataSize);
			mesh.Material = manager.GetAsset<Mule::Material>(name);
			modified |= true;
		}
		ImGui::EndDragDropTarget();
	}

	if (modified) entity.GetScene()->SetModified(true);
}

static void DisplayCamera(Mule::CameraComponent& camera, Mule::Entity entity)
{
	ImGui::Checkbox("Active", &camera.Active);
	ImGui::DragFloat("FOV Degrees", &camera.FOVDegrees, 0.01f, 1.f, 180.f);
	ImGui::DragFloat("Near Plane", &camera.NearPlane, 0.01f, 0.001f, 1000.f);
	ImGui::DragFloat("Far Plane", &camera.FarPlane, 0.01f, 0.001f, 10000.f);
	ImGui::InputFloat3("Up Dir", &camera.Up[0]);
}