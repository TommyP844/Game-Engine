#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include <string>
#include <vector>

#include "Entity.h"
#include "GUID.h"
#include "Mule/Renderer/Shader.h"
#include "Mule/Renderer/Material.h"
#include "Mule/Renderer/MeshCollection.h"

namespace Mule {

	struct TransformComponent
	{
		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;

		glm::vec3 Translation;
		glm::quat Orientation = glm::identity<glm::quat>();
		glm::vec3 Scale = glm::vec3(1.f, 1.f, 1.f);
	};

	struct TagComponent
	{
		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag) : Tag(tag) {}

		std::string Tag;
	};

	struct GUIDComponent
	{
		GUIDComponent() = default;
		GUIDComponent(const GUIDComponent&) = default;
		GUIDComponent(GUID guid) : GUID(guid) {}

		GUID GUID;
	};

	struct RelationshipComponent
	{
		RelationshipComponent() = default;
		RelationshipComponent(const RelationshipComponent& other) = default;

		Entity Parent;
		std::vector<Entity> Children;
	};

	struct MeshCollectionComponent
	{
		MeshCollectionComponent() = default;
		MeshCollectionComponent(const MeshCollectionComponent& other) = default;
		
		bool Visible = true;
		Ref<MeshCollection> Meshes;
		Ref<Shader> Shader = Shader::GetDefault();
		Ref<Material> Material = Material::GetDefault();
	};

	struct CameraComponent
	{
		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;

		bool Active = true;
		glm::mat4 Proj = glm::identity<glm::mat4>();
		glm::mat4 View = glm::identity<glm::mat4>();
		glm::vec3 Up = glm::vec3(0.f, 1.f, 0.f);
		glm::vec3 ForwardDir = glm::vec3(0.f, 0.f, -1.f);
		glm::vec3 RightDir;
		float FOVDegrees = 90.f;
		float NearPlane = 1.f;
		float FarPlane = 1000.f;
	};

	struct PointLightComponent
	{
		PointLightComponent() = default;
		PointLightComponent(const PointLightComponent&) = default;

		glm::vec3 Color;
		glm::vec3 Radius;
	};

}

