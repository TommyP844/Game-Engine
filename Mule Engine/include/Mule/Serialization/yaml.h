#pragma once

#include "Transform.h"
#include "ECS/Components.h"

#include <yaml-cpp/yaml.h>
#include <glm.hpp>
#include <gtx/quaternion.hpp>

namespace YAML
{
	// Literals

	template<>
	struct convert<glm::vec2>
	{
		static Node encode(const glm::vec2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			return node;
		}

		static bool decode(const Node& node, glm::vec2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::quat>
	{
		static Node encode(const glm::quat& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			return node;
		}

		static bool decode(const Node& node, glm::quat& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<Mule::Guid>
	{
		static Node encode(const Mule::Guid& rhs)
		{
			Node node;
			node = (size_t)rhs;
			return node;
		}

		static bool decode(const Node& node, Mule::Guid& rhs)
		{
			rhs = Mule::Guid(node.as<size_t>());
			return true;
		}
	};

	template<>
	struct convert<Mule::Transform>
	{
		static Node encode(const Mule::Transform& rhs)
		{
			Node node;
			node["Translation"] = rhs.Translation;
			node["Orientation"] = rhs.Rotation;
			node["Scale"] = rhs.Scale;
			return node;
		}

		static bool decode(const Node& node, Mule::Transform& rhs)
		{
			rhs.Translation = node["Translation"].as<glm::vec3>();
			rhs.Rotation = node["Orientation"].as<glm::vec3>();
			rhs.Scale = node["Scale"].as<glm::vec3>();

			return true;
		}
	};


	template<>
	struct convert<Mule::CameraComponent>
	{
		static Node encode(const Mule::CameraComponent& rhs)
		{
			Node node;

			node["Name"] = "CameraComponent";
			node["Primary"] = rhs.Primary;
			node["AspectRatio"] = rhs.Camera.GetAspectRatio();
			node["FarPlane"] = rhs.Camera.GetFarPlane();
			node["NearPlane"] = rhs.Camera.GetNearPlane();
			node["Pitch"] = rhs.Camera.GetPitchDegrees();
			node["Yaw"] = rhs.Camera.GetYawDegrees();
			node["UpDirection"] = rhs.Camera.GetWorldUp();
			node["FOVDegrees"] = rhs.Camera.GetFOVDegrees();

			return node;
		}

		static bool decode(const Node& node, Mule::CameraComponent& rhs)
		{
			rhs.Primary = node["Primary"].as<bool>();

			float aspectRatio = node["AspectRatio"].as<float>();
			float nearPlane = node["NearPlane"].as<float>();
			float farPlane = node["FarPlane"].as<float>();
			float pitch = node["Pitch"].as<float>();
			float yaw = node["Yaw"].as<float>();
			glm::vec3 up = node["UpDirection"].as<glm::vec3>();
			float fov = node["FOVDegrees"].as<float>();

			rhs.Camera = Mule::Camera();
			rhs.Camera.SetAspectRatio(aspectRatio);
			rhs.Camera.SetNearPlane(nearPlane);
			rhs.Camera.SetFarPlane(farPlane);
			rhs.Camera.SetPitchDegrees(pitch);
			rhs.Camera.SetYawDegrees(yaw);
			rhs.Camera.SetWorldUp(up);
			rhs.Camera.SetFOVDegrees(fov);


			return true;
		}
	};

	template<>
	struct convert<Mule::DirectionalLightComponent>
	{
		static Node encode(const Mule::DirectionalLightComponent& rhs)
		{
			Node node;

			node["Name"] = "DirectionalLightComponent";
			node["Color"] = rhs.Color;
			node["Intensity"] = rhs.Intensity;

			return node;
		}

		static bool decode(const Node& node, Mule::DirectionalLightComponent& rhs)
		{
			rhs.Color = node["Color"].as<glm::vec3>();
			rhs.Intensity = node["Intensity"].as<float>();

			return true;
		}
	};

	template<>
	struct convert<Mule::SpotLightComponent>
	{
		static Node encode(const Mule::SpotLightComponent& rhs)
		{
			Node node;

			node["Name"] = "SpotLightComponent";
			node["Color"] = rhs.Color;
			node["Intensity"] = rhs.Intensity;

			return node;
		}

		static bool decode(const Node& node, Mule::SpotLightComponent& rhs)
		{
			rhs.Color = node["Color"].as<glm::vec3>();
			rhs.Intensity = node["Intensity"].as<float>();

			return true;
		}
	};

	template<>
	struct convert<Mule::PointLightComponent>
	{
		static Node encode(const Mule::PointLightComponent& rhs)
		{
			Node node;

			node["Name"] = "PointLightComponent";
			node["Color"] = rhs.Color;
			node["Intensity"] = rhs.Intensity;

			return node;
		}

		static bool decode(const Node& node, Mule::PointLightComponent& rhs)
		{
			rhs.Color = node["Color"].as<glm::vec3>();
			rhs.Intensity = node["Intensity"].as<float>();

			return true;
		}
	};
}