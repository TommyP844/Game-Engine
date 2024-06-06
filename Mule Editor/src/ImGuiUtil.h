
#pragma once

#include <imgui.h>
#include <glm.hpp>
#include <gtx/quaternion.hpp>

#include <string>
#include <filesystem>

#include <Mule.h>

namespace fs = std::filesystem;

constexpr const char* SCENE_HANDLE_PAYLOAD = "Scene-Handle";
constexpr const char* SCENE_PATH_PAYLOAD = "Scene-Path";
constexpr const char* MESH_HANDLE_PAYLOAD = "Mesh-Handle";
constexpr const char* MODEL_HANDLE_PAYLOAD = "Model-Handle";
constexpr const char* MODEL_PATH_PAYLOAD = "Model-Path";

constexpr const char* ENTITY_ID_PAYLOAD = "Entity-ID";

#define VEC3_INPUT_WIDTH 80.f

bool Vec3Display(const std::string& name, glm::vec3& vec, const glm::vec3& reset = { 0.f, 0.f, 0.f });

void DragDropTarget(const std::string& type, std::function<void(const ImGuiPayload*)> func);
void DragDropTarget(const std::string& type, std::function<void(const fs::path&)> func);
void DragDropTarget(const std::string& type, std::function<void(Mule::AssetHandle)> func);

void DragDropSource(const std::string& type, void* data, int size);
void DragDropSource(const std::string& type, const std::string& str);
void DragDropSource(const std::string& type, const fs::path& path);

void DragDropModelToEntity(Mule::Entity e);

std::string StrFromPayload(const ImGuiPayload* payload);
fs::path PathFromPayload(const ImGuiPayload* payload);

template<typename T>
T TypeFromPayload(const ImGuiPayload* payload)
{
	T data = *(T*)payload->Data;
	return data;
}
