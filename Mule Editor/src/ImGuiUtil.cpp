#include "ImGuiUtil.h"


bool Vec3Display(const std::string& name, glm::vec3& vec, const glm::vec3& reset)
{
	bool modified = false;
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.f, 0.f });

	// X
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.f, 0.f, 1.f));
	if (ImGui::Button(("X##" + name).c_str())) vec.x = reset.x;
	ImGui::SameLine();
	ImGui::PushItemWidth(VEC3_INPUT_WIDTH);
	modified |= ImGui::DragFloat(("##" + name + "X").c_str(), &vec.x);
	ImGui::SameLine();

	// Y
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 1.f, 0.f, 1.f));
	if (ImGui::Button(("Y##" + name).c_str())) vec.y = reset.y;
	ImGui::SameLine();
	ImGui::PushItemWidth(VEC3_INPUT_WIDTH);
	modified |= ImGui::DragFloat(("##" + name + "Y").c_str(), &vec.y);
	ImGui::SameLine();

	// Z
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 1.f, 1.f));
	if (ImGui::Button(("Z##" + name).c_str())) vec.z = reset.z;
	ImGui::SameLine();
	ImGui::PushItemWidth(VEC3_INPUT_WIDTH);
	modified |= ImGui::DragFloat(("##" + name + "Z").c_str(), &vec.z);

	ImGui::PopStyleColor(3);
	ImGui::PopStyleVar();

	return modified;
}

void Vec3DisplayDegrees(const std::string& name, glm::quat& quat)
{
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.f, 0.f });

	glm::vec3 euler = glm::degrees(glm::eulerAngles(quat));

	// X
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.f, 0.f, 1.f));
	if (ImGui::Button(("X##" + name).c_str())) euler.x = 0.f;
	ImGui::SameLine();
	ImGui::PushItemWidth(VEC3_INPUT_WIDTH);
	ImGui::DragFloat(("##" + name + "X").c_str(), &euler.x);
	ImGui::SameLine();

	// Y
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 1.f, 0.f, 1.f));
	if (ImGui::Button(("Y##" + name).c_str())) euler.y = 0.f;
	ImGui::SameLine();
	ImGui::PushItemWidth(VEC3_INPUT_WIDTH);
	ImGui::DragFloat(("##" + name + "Y").c_str(), &euler.y);
	ImGui::SameLine();

	// Z
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 1.f, 1.f));
	if (ImGui::Button(("Z##" + name).c_str())) euler.z = 0.f;
	ImGui::SameLine();
	ImGui::PushItemWidth(VEC3_INPUT_WIDTH);
	ImGui::DragFloat(("##" + name + "Z").c_str(), &euler.z);

	ImGui::PopStyleColor(3);
	ImGui::PopStyleVar();

	quat = glm::quat(glm::radians(euler));
}

void DragDropTarget(const std::string& type, std::function<void(const ImGuiPayload*)> func)
{
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(type.c_str()))
		{
			func(payload);
		}
		ImGui::EndDragDropTarget();
	}
}

void DragDropTarget(const std::string& type, std::function<void(const fs::path&)> func)
{
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(type.c_str()))
		{
			fs::path path = PathFromPayload(payload);
			func(path);
		}
		ImGui::EndDragDropTarget();
	}
}

void DragDropTarget(const std::string& type, std::function<void(Mule::AssetHandle)> func)
{
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(type.c_str()))
		{
			Mule::AssetHandle handle = *(Mule::AssetHandle*)payload->Data;
			func(handle);
		}
		ImGui::EndDragDropTarget();
	}
}

void DragDropTarget(const std::vector<std::string>& types, std::function<void(const fs::path&)> func)
{
	for (const auto& type : types)
	{
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(type.c_str()))
			{
				fs::path path = PathFromPayload(payload);
				func(path);
			}
			ImGui::EndDragDropTarget();
		}
	}
}

std::string StrFromPayload(const ImGuiPayload* payload)
{
	return std::string((char*)payload->Data, payload->DataSize);
}

fs::path PathFromPayload(const ImGuiPayload* payload)
{
	return std::string((char*)payload->Data, payload->DataSize);
}

bool DragDropSource(const std::string& type, void* data, int size, bool allowNullId, std::function<void()> tooltipFunc)
{
	bool ret = false;
	if (ImGui::BeginDragDropSource(allowNullId ? ImGuiDragDropFlags_SourceAllowNullID : 0))
	{
		ret = ImGui::SetDragDropPayload(type.c_str(), data, size);
		if (tooltipFunc)
			tooltipFunc();
		ImGui::EndDragDropSource();
	}
	return ret;
}

bool DragDropSource(const std::string& type, const std::string& str, bool allowNullId, std::function<void()> tooltipFunc)
{
	bool ret = false;
	if (ImGui::BeginDragDropSource(allowNullId ? ImGuiDragDropFlags_SourceAllowNullID : 0))
	{
		ret = ImGui::SetDragDropPayload(type.c_str(), str.data(), str.size());
		if (tooltipFunc)
			tooltipFunc();
		ImGui::EndDragDropSource();
	}
	return ret;
}

bool DragDropSource(const std::string& type, const fs::path& path, std::function<void()> tooltipFunc)
{
	bool ret = false;
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
	{
		ret = ImGui::SetDragDropPayload(type.c_str(), path.string().data(), path.string().size());
		if (tooltipFunc)
			tooltipFunc();
		ImGui::EndDragDropSource();
	}
	return ret;
}

void RecusivlyApplyModelNodeToEntity(Mule::Entity entity, const Mule::MeshNode& node)
{
	if (!entity.HasComponent<Mule::MeshCollectionComponent>())
		entity.AddComponent<Mule::MeshCollectionComponent>();
	auto& collection = entity.GetComponent<Mule::MeshCollectionComponent>();
	for (auto& mesh : node.Meshes)
	{
		Mule::MeshComponent meshComponent;

		meshComponent.Visible = true;
		meshComponent.ReceiveShadows = true;
		meshComponent.CastsShadows = true;
		meshComponent.ShaderHandle = Mule::Asset::NullHandle;
		meshComponent.MeshHandle = mesh;
		// TODO: get mesh from manager and apply material
		//meshComponent.MaterialHandle = mesh->GetDefaultMaterialHandle();

		collection.Meshes.push_back(meshComponent);
	}

	for (auto& childNode : node.Children)
	{
		auto child = entity.GetScene()->CreateEntity();
		entity.AddChild(child);
		RecusivlyApplyModelNodeToEntity(child, childNode);
	}

}

void DragDropModelToEntity(Mule::Entity e)
{
	if (ImGui::BeginDragDropTarget())
	{
		Mule::AssetManager& manager = Mule::AssetManager::Get();
		Mule::Ref<Mule::ModelDescription> modelDesc;
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(MODEL_PATH_PAYLOAD))
		{
			fs::path path = PathFromPayload(payload);
			modelDesc = manager.GetAssetByFilepath<Mule::ModelDescription>(path);
		}

		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(MODEL_HANDLE_PAYLOAD))
		{
			Mule::AssetHandle handle = *(Mule::AssetHandle*)payload->Data;
			modelDesc = manager.GetAsset<Mule::ModelDescription>(handle);
		}
		ImGui::EndDragDropTarget();

		if (!modelDesc) return;

		RecusivlyApplyModelNodeToEntity(e, modelDesc->Root);

	}
}
