#pragma once

#include <Mule.h>

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

static bool DisplayCameraComponent(Mule::Entity e, Mule::CameraComponent& cameraComponent)
{
	bool modified = false;
	//
	//auto& camera = cameraComponent.Camera;
	//
	//modified |= ValueEdit("FOV", [&]() {
	//	float fov = camera.GetFOVDegrees();
	//	if (ImGui::DragFloat("##CameraFOV", &fov, 1.f, 1.f, 270.f, "%.2f degs", ImGuiSliderFlags_AlwaysClamp))
	//	{
	//		camera.SetFOV(fov);
	//		return true;
	//	}
	//	return false;
	//	});
	//
	//modified |= ValueEdit("NearPlane", [&]() {
	//	float near = camera.NearPlane();
	//	if (ImGui::DragFloat("##CameraNearPlane", &near, 1.f, 0.001f, camera.FarPlane() - 1.f, "%.2f", ImGuiSliderFlags_AlwaysClamp))
	//	{
	//		camera.SetNearPlane(near);
	//		return true;
	//	}
	//	return false;
	//	});
	//
	//modified |= ValueEdit("Far Plane", [&]() {
	//	float far = camera.FarPlane();
	//	if (ImGui::DragFloat("##CameraFarPlane", &far, 1.f, camera.NearPlane() + 1.f, 10000.f, "%.2f", ImGuiSliderFlags_AlwaysClamp))
	//	{
	//		camera.SetFarPlane(far);
	//		return true;
	//	}
	//	return false;
	//	});
	//
	//modified |= ValueEdit("Yaw", [&]() {
	//	float yaw = camera.YawDegrees();
	//	if (ImGui::DragFloat("##CameraYaw", &yaw, 1.f, 0.f, 359.99f, "%.2f degs", ImGuiSliderFlags_AlwaysClamp))
	//	{
	//		camera.SetYaw(yaw);
	//		return true;
	//	}
	//	return false;
	//	});
	//
	//modified |= ValueEdit("Pitch", [&]() {
	//	float pitch = camera.PitchDegrees();
	//	if (ImGui::DragFloat("##CameraPitch", &pitch, 1.f, -90.f, 90.f, "%.2f degs", ImGuiSliderFlags_AlwaysClamp))
	//	{
	//		camera.SetPitch(pitch);
	//		return true;
	//	}
	//	return false;
	//	});
	//
	return modified;
}

static bool DisplayModelComponent(Mule::Entity e, Mule::ModelComponent& model)
{
	bool modified = false;
	Mule::AssetManager& manager = Mule::AssetManager::Get();
	
	//modified |= ValueEdit("Model", [&]() {
	//	bool modified = false;
	//	std::string modelName = "(Empty)";
	//	Mule::Ref<Mule::ModelDesc> modelDesc = nullptr;
	//	if (modelDesc = manager.GetAsset<Mule::ModelDesc>(model.ModelHandle))
	//	{
	//		modelName = modelDesc->Name();
	//	}
	//	ImGui::Text(modelName.c_str());
	//	DRAG_DROP_TARGET(DRAG_DROP_MODEL_FILE, [&](const ImGuiPayload* payload) {
	//		std::string path = std::string((char*)payload->Data, payload->DataSize);
	//		Mule::Ref<Mule::ModelDesc> modelAsset = manager.GetAssetByFilepath<Mule::ModelDesc>(path);
	//		if (!modelAsset)
	//		{
	//			modelAsset = LoadMeshAsync(path, e);
	//		}
	//		model.ModelHandle = modelAsset->Handle();
	//		model.MeshData.clear();
	//		for (auto meshHandle : modelAsset->MeshHandles)
	//		{
	//			if (auto mesh = manager.GetAsset<Mule::Mesh>(meshHandle))
	//			{
	//				Mule::ModelComponentMesh meshData{};
	//
	//				meshData.MeshHandle = meshHandle;
	//				meshData.MaterialHandle = mesh->DefaultMaterialHandle();
	//				meshData.CastsShadows = true;
	//				meshData.ReceiveShadows = true;
	//				meshData.Visible = true;
	//
	//				model.MeshData.push_back(meshData);
	//			}
	//		}
	//	});
	//	return modified;
	//});
	//
	//modified |= ValueEdit("Meshes", [&]() {
	//	bool modified = false;
	//	if (ImGui::TreeNode("View"))
	//	{
	//		for (int i = 0; i < model.MeshData.size(); i++)
	//		{
	//			auto& meshData = model.MeshData[i];
	//			auto mesh = manager.GetAsset<Mule::Mesh>(meshData.MeshHandle);
	//			std::string materialName = "(Unknown)";
	//			if (auto material = manager.GetAsset<Mule::Material>(meshData.MaterialHandle))
	//			{
	//				if (mesh)
	//				{
	//					if (material->Handle() == mesh->DefaultMaterialHandle())
	//						materialName = "(Mesh Default)";
	//					else
	//						materialName = material->Name();
	//				}
	//			}
	//			std::string meshName = "(Not Loaded)";
	//			if (mesh)
	//			{
	//				meshName = mesh->Name();
	//			}
	//			ImGui::Text("Name: %s", meshName.c_str());
	//			ImGui::Text("Material: %s", materialName.c_str());
	//			DRAG_DROP_TARGET(DRAG_DROP_MATERIAL_FILE, [&](const ImGuiPayload* payload) {
	//				std::string path = PAYLOAD_TO_STR(payload);
	//				if (auto material = manager.GetAssetByFilepath<Mule::Material>(path))
	//				{
	//					meshData.MaterialHandle = material->Handle();
	//				}
	//				else
	//				{
	//					auto loadedMaterial = Mule::Material::Load(path);
	//					manager.InsertAsset(loadedMaterial);
	//					meshData.MaterialHandle = loadedMaterial->Handle();
	//				}
	//				});
	//			int vertices = 0;
	//			int indices = 0;
	//			if (mesh)
	//			{
	//				vertices = mesh->NumVertices();
	//				indices = mesh->NumIndices();
	//			}
	//			ImGui::Text("Vertices: %i", vertices);
	//			ImGui::Text("Indices: %i", indices);	
	//			modified |= ImGui::Checkbox("Visible", &meshData.Visible);
	//			modified |= ImGui::Checkbox("Casts Shadows", &meshData.CastsShadows);
	//			modified |= ImGui::Checkbox("Receive Shadows", &meshData.ReceiveShadows);
	//
	//			if (i != model.MeshData.size() - 1)
	//				ImGui::Separator();
	//		}
	//		ImGui::TreePop();
	//	}
	//	return modified;
	//});

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
		return ImGui::Checkbox("##DirectionalLightVisible", &directionalLight.Visible);
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

static bool DisplaySpotLightComponent(Mule::Entity e, Mule::SpotLightComponent& spotLight)
{
	bool modified = false;

	modified |= ValueEdit("Visible", [&]() {
		return ImGui::Checkbox("##SpotLightVisible", &spotLight.Visible);
		});

	modified |= ValueEdit("Color", [&]() {
		ImVec4& color = *(ImVec4*)&spotLight.Color; // TODO: double check this works
		return ImGui::ColorButton("##SpotLightColor", color, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_Float);
		});

	modified |= ValueEdit("Angle", [&]() {
		return ImGui::DragFloat("##SpotLightAngle", &spotLight.Theta, 1.f, 0.1f, 359.f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
		});

	modified |= ValueEdit("Intensity", [&]() {
		return ImGui::DragFloat("##SpotLightIntensity", &spotLight.Intensity, 1.f, 0.0f, 1000.f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
		});

	modified |= ValueEdit("Att. Constant", [&]() {
		return ImGui::DragFloat("##SpotLightAttConstant", &spotLight.AttenuationConstant, 1.f, 0.0f, 1000.f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
		});

	modified |= ValueEdit("Att. Linear", [&]() {
		return ImGui::DragFloat("##SpotLightAttLinear", &spotLight.AttenuationLinear, 1.f, 0.0f, 1000.f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
		});

	modified |= ValueEdit("Att. Quadratic", [&]() {
		return ImGui::DragFloat("##SpotLightAttQuadratic", &spotLight.AttenuationQuadratic, 1.f, 0.0f, 1000.f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
		});

	return modified;
}

static bool DisplayPointLightComponent(Mule::Entity e, Mule::PointLightComponent& pointLight)
{
	bool modified = false;

	
	modified |= ValueEdit("Visible", [&]() {
		return ImGui::Checkbox("##DirectionalLightVisible", &pointLight.Visible);
		});

	modified |= ValueEdit("Color", [&]() {
		ImVec4& color = *(ImVec4*)&pointLight.Color; // TODO: double check this works
		return ImGui::ColorButton("##PointLightColor", color, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_Float);
		});

	modified |= ValueEdit("Intensity", [&]() {
		return ImGui::DragFloat("##PointLightIntensity", &pointLight.Intensity, 1.f, 0.0f, 1000.f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
		});

	modified |= ValueEdit("Att. Constant", [&]() {
		return ImGui::DragFloat("##PointLightAttConstant", &pointLight.AttenuationConstant, 1.f, 0.0f, 1000.f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
		});

	modified |= ValueEdit("Att. Linear", [&]() {
		return ImGui::DragFloat("##PointLightAttLinear", &pointLight.AttenuationLinear, 1.f, 0.0f, 1000.f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
		});

	modified |= ValueEdit("Att. Quadratic", [&]() {
		return ImGui::DragFloat("##PointLightAttQuadratic", &pointLight.AttenuationQuadratic, 1.f, 0.0f, 1000.f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
		});

	return modified;
}