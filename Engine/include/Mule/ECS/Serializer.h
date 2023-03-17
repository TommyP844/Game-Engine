#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include <yaml-cpp/yaml.h>

#include "Components.h"

namespace Mule
{
	void SerializeMaterial(const fs::path& path, Ref<Material> material);

	Ref<Material> DeSerializeMaterial(const fs::path& path);

	static YAML::Node SerializeVec2(const glm::vec2& vec)
	{
		YAML::Node node;

		node.push_back(vec.x);
		node.push_back(vec.y);

		return node;
	}

	static glm::vec2 DeSerializeVec2(const YAML::Node& node)
	{
		glm::vec2 vec;

		vec.x = node[0].as<float>();
		vec.y = node[1].as<float>();

		return vec;
	}

	static YAML::Node SerializeVec3(const glm::vec3& vec)
	{
		YAML::Node node;

		node.push_back(vec.x);
		node.push_back(vec.y);
		node.push_back(vec.z);

		return node;
	}

	static glm::vec3 DeSerializeVec3(const YAML::Node& node)
	{
		glm::vec3 vec;

		vec.x = node[0].as<float>();
		vec.y = node[1].as<float>();
		vec.z = node[2].as<float>();

		return vec;
	}

	static YAML::Node SerializeVec4(const glm::vec4& vec)
	{
		YAML::Node node;

		node.push_back(vec.x);
		node.push_back(vec.y);
		node.push_back(vec.z);
		node.push_back(vec.w);

		return node;
	}

	static glm::vec4 DeSerializeVec4(const YAML::Node& node)
	{
		glm::vec4 vec;

		vec.x = node[0].as<float>();
		vec.y = node[1].as<float>();
		vec.z = node[2].as<float>();
		vec.w = node[3].as<float>();

		return vec;
	}

	static YAML::Node SerializeQuat(const glm::quat& vec)
	{
		YAML::Node node;

		node.push_back(vec.x);
		node.push_back(vec.y);
		node.push_back(vec.z);
		node.push_back(vec.w);

		return node;
	}

	static glm::quat DeSerializeQuat(const YAML::Node& node)
	{
		glm::quat vec;

		vec.x = node[0].as<float>();
		vec.y = node[1].as<float>();
		vec.z = node[2].as<float>();
		vec.w = node[3].as<float>();

		return vec;
	}


	/*
	*			Tag Component
	*/
	static YAML::Node SerializeTag(const TagComponent& tag)
	{
		YAML::Node node;

		node["Tag"] = tag.Tag;

		return node;
	}

	static void DeSerializeTag(TagComponent& tag, const YAML::Node& node)
	{
		tag.Tag = node["Tag"].as<std::string>();
	}


	/*
	*		GUID Component
	*/
	static YAML::Node SerializeGuid(const GUIDComponent& guid)
	{
		YAML::Node node;

		node["Guid"] = guid.GUID.AsULL();

		return node;
	}

	static void DeSerializeGuid(GUIDComponent& guid, const YAML::Node& node)
	{
		guid.GUID = GUID(node["Guid"].as<std::uint64_t>());
	}


	/*
	*		Transform Component
	*/
	static YAML::Node SerializeTransform(const TransformComponent& transform)
	{
		YAML::Node node;

		node["Position"] = SerializeVec3(transform.Translation);
		node["Orientation"] = SerializeQuat(transform.Orientation);
		node["Scale"] = SerializeVec3(transform.Scale);

		return node;
	}

	static void DeSerializeTransform(TransformComponent& transform, const YAML::Node& node)
	{
		transform.Translation = DeSerializeVec3(node["Position"]);
		transform.Orientation = DeSerializeQuat(node["Orientation"]);
		transform.Scale = DeSerializeVec3(node["Scale"]);
	}

}
