#include "ComponentPanel.h"

#include "imgui.h"

#include "Mule/Mule.h"

#include "ComponentDisplay.h"


ComponentPanel::~ComponentPanel()
{
}

void ComponentPanel::OnAttach()
{
}

static void Vec3Display(const std::string& label, glm::vec3& vec, float min = -FLT_MAX, float max = FLT_MAX)
{
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
	ImGui::PushItemWidth(50.f);

	ImGui::PushStyleColor(ImGuiCol_Button, { 1.f, 0.f, 0.f, 1.f });

	ImGui::Text(label.c_str());
	ImGui::SameLine(110.f);

	if (ImGui::Button("X")) vec.x = 0.f;
	ImGui::PopStyleColor();
	ImGui::SameLine();
	ImGui::DragFloat(std::string("##X" + label).c_str(), &vec.x, 1.f, min, max, "%.2f", ImGuiSliderFlags_NoRoundToFormat);
	ImGui::SameLine();
	
	ImGui::PushStyleColor(ImGuiCol_Button, { 0.f, 1.f, 0.f, 1.f });
	if (ImGui::Button("Y")) vec.y = 0.f;
	ImGui::PopStyleColor();
	ImGui::SameLine();
	ImGui::DragFloat(std::string("##Y" + label).c_str(), &vec.y, 1.f, min, max, "%.2f", ImGuiSliderFlags_NoRoundToFormat);
	ImGui::SameLine();
	
	ImGui::PushStyleColor(ImGuiCol_Button, { 0.f, 0.f, 1.f, 1.f });
	if (ImGui::Button("Z")) vec.z = 0.f;
	ImGui::PopStyleColor();
	ImGui::SameLine();
	ImGui::DragFloat(std::string("##Z" + label).c_str(), &vec.z, 1.f, min, max, "%.2f", ImGuiSliderFlags_NoRoundToFormat);

	ImGui::PopItemWidth();
	ImGui::PopStyleVar(2);

}

void ComponentPanel::OnUpdate()
{
	
	if (ImGui::Begin("Component Panel"))
	{
		if (mAppData.SelectedEntities.empty())
		{
			ImGui::End();
			return;
		}

		// TODO: implement multiselection
		if (mAppData.SelectedEntities.size() > 1)
		{
			ImGui::End();
			return;
		}

		Mule::Entity selected = mAppData.SelectedEntities[0];

		// Components
		Mule::TagComponent& tagComponent = selected.GetComponent<Mule::TagComponent>();
		Mule::TransformComponent& transformComponent = selected.GetComponent<Mule::TransformComponent>();

		static const char* components[] = {
			"Mesh",
			"Mesh Renderer"
		};

		static const char* selected_item = components[0];

		static bool showPopup = false;

		if (ImGui::Button("Add Component"))	ImGui::OpenPopup("AddComponents");

		if (ImGui::BeginPopupContextItem("AddComponents"))
		{
			if (ImGui::MenuItem("Camera"))
			{
				selected.AddComponent<Mule::CameraComponent>();
			}
			if (ImGui::MenuItem("Mesh Collection"))
			{
				selected.AddComponent<Mule::MeshCollectionComponent>();
			}
			ImGui::EndPopup();
		}

		ImGui::Text("GUID: %llu", selected.GetGUID());

		char buffer[256] = { 0 };
		std::string tag = selected.GetTag();
		memcpy(buffer, tag.data(), tag.size());
		if (ImGui::InputText("Tag", buffer, 256, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			tagComponent.Tag = buffer;
		}

		Vec3Display("Translation", transformComponent.Translation);
		
		glm::vec3 eulerAngles = glm::degrees(glm::eulerAngles(transformComponent.Orientation));
		Vec3Display("Rotation", eulerAngles, -359.99f, 360.f);
		transformComponent.Orientation = glm::quat(glm::radians(eulerAngles));

		Vec3Display("Scale", transformComponent.Scale);

		ImGui::Separator();
		
		DisplayComponent<Mule::CameraComponent>("Camera", selected, DisplayCamera);
		DisplayComponent<Mule::MeshCollectionComponent>("Mesh Collection", selected, DisplayMeshCollection);

	}
	ImGui::End();
}

void ComponentPanel::OnDetach()
{
}
