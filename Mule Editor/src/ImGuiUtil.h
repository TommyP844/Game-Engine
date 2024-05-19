#pragma once

#include <imgui.h>
#include <glm.hpp>
#include <gtx/quaternion.hpp>

#include <string>

#define VEC3_INPUT_WIDTH 80.f

static void Vec3Display(const std::string& name, glm::vec3& vec, const glm::vec3& reset = { 0.f, 0.f, 0.f })
{
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.f, 0.f });

	// X
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.f, 0.f, 1.f));
	if (ImGui::Button(("X##" + name).c_str())) vec.x = reset.x;
	ImGui::SameLine();
	ImGui::PushItemWidth(VEC3_INPUT_WIDTH);
	ImGui::DragFloat(("##" + name + "X").c_str(), &vec.x);
	ImGui::SameLine();

	// Y
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 1.f, 0.f, 1.f));
	if (ImGui::Button(("Y##" + name).c_str())) vec.y = reset.y;
	ImGui::SameLine();
	ImGui::PushItemWidth(VEC3_INPUT_WIDTH);
	ImGui::DragFloat(("##" + name + "Y").c_str(), &vec.y);
	ImGui::SameLine();

	// Z
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 1.f, 1.f));
	if (ImGui::Button(("Z##" + name).c_str())) vec.z = reset.z;
	ImGui::SameLine();
	ImGui::PushItemWidth(VEC3_INPUT_WIDTH);
	ImGui::DragFloat(("##" + name + "Z").c_str(), &vec.z);

	ImGui::PopStyleColor(3);
	ImGui::PopStyleVar();
}

static void Vec3DisplayDegrees(const std::string& name, glm::quat& quat)
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