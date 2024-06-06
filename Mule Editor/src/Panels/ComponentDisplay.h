#pragma once

#include <Mule.h>
#include "ImGuiUtil.h"

static bool ValueEdit(const std::string& name, std::function<bool()> func)
{
	ImGui::TableNextRow();
	ImGui::TableNextColumn();
	ImGui::Text(name.c_str());
	ImGui::TableNextColumn();
	return func();
}

template<typename T>
static bool ComponentEdit(Mule::Entity entity, const std::string& name, std::function<bool(Mule::Entity, T&)> func)
{
	bool modified = false;
	if (entity.HasComponent<T>())
	{
		bool open = ImGui::TreeNode(name.c_str());
		if (ImGui::BeginPopupContextItem(name.c_str()))
		{
			if (ImGui::MenuItem("Remove"))
			{
				entity.RemoveComponent<T>();
			}
			ImGui::EndPopup();
		}
		if(open)
		{
			if (ImGui::BeginTable("#componentValues", 2))
			{
				modified = func(entity, entity.GetComponent<T>());
				ImGui::EndTable();
			}
			ImGui::Separator();
			ImGui::TreePop();
		}
	}
	return modified;
}

template<typename T>
static bool CollectionEdit(Mule::Entity entity, const std::string& name, T& component, bool& remove, std::function<bool(Mule::Entity, T&)> func)
{
	bool modified = false;
	bool open = ImGui::TreeNode(name.c_str());
	if (ImGui::BeginPopupContextItem(name.c_str()))
	{
		if (ImGui::MenuItem("Remove"))
		{
			remove = true;
		}
		ImGui::EndPopup();
	}
	if (open)
	{
		if (ImGui::BeginTable("#componentValues", 2))
		{
			modified = func(entity, component);
			ImGui::EndTable();
		}
		ImGui::Separator();
		ImGui::TreePop();
	}

	return modified;
}

static bool DisplayCameraComponent(Mule::Entity e, Mule::CameraComponent& cameraComponent)
{
	bool modified = false;
	
	auto& camera = cameraComponent.Camera;
	
	modified |= ValueEdit("Primary", [&]() {
		if (ImGui::Checkbox("##CameraPrimary", &cameraComponent.Primary))
		{
			return true;
		}
		return false;
		});

	modified |= ValueEdit("FOV", [&]() {
		float fov = camera.GetFOVDegrees();
		if (ImGui::DragFloat("##CameraFOV", &fov, 1.f, 1.f, 270.f, "%.2f degs", ImGuiSliderFlags_AlwaysClamp))
		{
			camera.SetFOVDegrees(fov);
			return true;
		}
		return false;
		});
	
	modified |= ValueEdit("Near Plane", [&]() {
		float nearPlane = camera.GetNearPlane();
		if (ImGui::DragFloat("##CameraNearPlane", &nearPlane, 1.f, 0.001f, camera.GetFarPlane() - 1.f, "%.2f", ImGuiSliderFlags_AlwaysClamp))
		{
			camera.SetNearPlane(nearPlane);
			return true;
		}
		return false;
		});
	
	modified |= ValueEdit("Far Plane", [&]() {
		float farPlane = camera.GetFarPlane();
		if (ImGui::DragFloat("##CameraFarPlane", &farPlane, 1.f, camera.GetFarPlane() + 1.f, 10000.f, "%.2f", ImGuiSliderFlags_AlwaysClamp))
		{
			camera.SetFarPlane(farPlane);
			return true;
		}
		return false;
		});

	modified |= ValueEdit("Yaw", [&]() {
		float yaw = camera.GetYawDegrees();
		if (ImGui::DragFloat("##CameraYaw", &yaw, 1.f, 0.f, 360.f, "%.2f", ImGuiSliderFlags_AlwaysClamp))
		{
			camera.SetYawDegrees(yaw);
			return true;
		}
		return false;
		});

	modified |= ValueEdit("Pitch", [&]() {
		float pitch = camera.GetPitchDegrees();
		if (ImGui::DragFloat("##CameraPitch", &pitch, 1.f, 0.f, 360.f, "%.2f", ImGuiSliderFlags_AlwaysClamp))
		{
			camera.SetPitchDegrees(pitch);
			return true;
		}
		return false;
		});
	
	return modified;
}

static bool DisplayScriptComponent(Mule::Entity e, Mule::ScriptComponent& script)
{
	bool modified = false;
	//modified |= ValueEdit("Script Class", [&]() {
	//	static std::string scriptName;
	//	bool modified = false;
	//	if (script.Class)
	//	{
	//		scriptName = script.Class->ClassName();
	//	}
	//	Mule::MonoEngine& engine = Mule::MonoEngine::Get();
	//	bool found = FilteredInputTextWithDropdown("##scriptNameInput", engine.GetClassNames(), scriptName);
	//	if (!script.Class && found)
	//	{
	//		script.Class = engine.CreateInstance(scriptName);
	//		modified = true;
	//	}
	//	return modified;
	//	});
	//if (script.Class)
	//{
	//	//ScriptFieldEdit(script.Class, mEditorState.GetActiveScene());
	//}
	return modified;
}

static bool DisplayDirectionalLightComponent(Mule::Entity e, Mule::DirectionalLightComponent& directionalLight)
{
	bool modified = false;

	modified |= ValueEdit("Visible", [&]() {
		return false;//return ImGui::Checkbox("##DirectionalLightVisible", &directionalLight.Visible);
		});

	modified |= ValueEdit("Color", [&]() {
		ImVec4& color = *(ImVec4*)&directionalLight.Color; // TODO: double check this works
		return ImGui::ColorButton("##DirectionalLightColor", color, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_Float);
		});

	modified |= ValueEdit("Intensity", [&]() {
		return ImGui::DragFloat("##DirectionalLightIntensity", &directionalLight.Intensity, 1.f, 0.0f, 1000.f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
		});

	return modified;
}

// Collections need unique ImGui ID
static bool DisplayMeshComponent(Mule::Entity e, Mule::MeshComponent& meshComponent)
{
	bool modified = false;
	Mule::AssetManager& manager = Mule::AssetManager::Get();
	
	modified |= ValueEdit("Visible", [&]() {
		return ImGui::Checkbox("##MeshVisible", &meshComponent.Visible);
		});

	modified |= ValueEdit("Casts Shadows", [&]() {
		return ImGui::Checkbox("##MeshCastShadows", &meshComponent.CastsShadows);
		});

	modified |= ValueEdit("Receive Shadows", [&]() {
		return ImGui::Checkbox("##MeshReceiveShadows", &meshComponent.ReceiveShadows);
		});

	modified |= ValueEdit("Material", [&]() {
		char buffer[256] = { 0 };
		if (ImGui::InputText("##MeshMaterial", buffer, 256, ImGuiInputTextFlags_EnterReturnsTrue))
		{

		}
		return false;
		});

	modified |= ValueEdit("Mesh", [&]() {
		std::string meshName = "(No Mesh)";
		auto asset = manager.GetAsset<Mule::Asset>(meshComponent.MeshHandle);
		if (asset)
		{
			meshName = asset->Name();
		}
		ImGui::Text(meshName.c_str());
		DragDropTarget(MESH_HANDLE_PAYLOAD, [&](Mule::AssetHandle handle) { meshComponent.MeshHandle = handle; });
		return false;
		});

	return modified;
}

static bool DisplaySpotLightComponent(Mule::Entity e, Mule::SpotLightComponent& spotLight)
{
	bool modified = false;

	modified |= ValueEdit("Visible", [&]() {
		return false;// ImGui::Checkbox("##SpotLightVisible", &spotLight.Visible);
		});

	modified |= ValueEdit("Color", [&]() {
		ImVec4& color = *(ImVec4*)&spotLight.Color; // TODO: double check this works
		return ImGui::ColorButton("##SpotLightColor", color, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_Float);
		});

	modified |= ValueEdit("Angle", [&]() {
		return ImGui::DragFloat("##SpotLightAngle", &spotLight.Angle, 1.f, 0.1f, 359.f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
		});

	modified |= ValueEdit("Intensity", [&]() {
		return ImGui::DragFloat("##SpotLightIntensity", &spotLight.Intensity, 1.f, 0.0f, 1000.f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
		});


	return modified;
}

static bool DisplayPointLightComponent(Mule::Entity e, Mule::PointLightComponent& pointLight)
{
	bool modified = false;
		
	modified |= ValueEdit("Visible", [&]() {
		return false;// ImGui::Checkbox("##DirectionalLightVisible", &pointLight.Visible);
		});

	modified |= ValueEdit("Color", [&]() {
		ImVec4& color = *(ImVec4*)&pointLight.Color; // TODO: double check this works
		return ImGui::ColorButton("##PointLightColor", color, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_Float);
		});

	modified |= ValueEdit("Intensity", [&]() {
		return ImGui::DragFloat("##PointLightIntensity", &pointLight.Intensity, 1.f, 0.0f, 1000.f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
		});

	return modified;
}