#pragma once

#include "yaml-cpp/yaml.h"

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Components.h"
#include "Entity.h"

#define COMPONENTS "Components"
#define TAG_COMPONENT "TagComponent"
#define GUID_COMPONENT "GUIDComponent"
#define TRANSFORM_COMPONENT "TransformComponent"


namespace YAML
{
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
			rhs.w = node[3].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::ivec2>
	{
		static Node encode(const glm::ivec2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			return node;
		}

		static bool decode(const Node& node, glm::ivec2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<int>();
			rhs.y = node[1].as<int>();
			return true;
		}
	};

	template<>
	struct convert<glm::ivec3>
	{
		static Node encode(const glm::ivec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}

		static bool decode(const Node& node, glm::ivec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<int>();
			rhs.y = node[1].as<int>();
			rhs.z = node[2].as<int>();
			return true;
		}
	};

	template<>
	struct convert<glm::ivec4>
	{
		static Node encode(const glm::ivec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			return node;
		}

		static bool decode(const Node& node, glm::ivec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<int>();
			rhs.y = node[1].as<int>();
			rhs.z = node[2].as<int>();
			rhs.w = node[3].as<int>();
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
			rhs.w = node[3].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::mat2>
	{
		static Node encode(const glm::mat2& rhs)
		{
			Node node;
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
					node.push_back(rhs[i][j]);
			return node;
		}

		static bool decode(const Node& node, glm::mat2& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
					rhs[i][j] = node[i * 2 + j].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::mat3>
	{
		static Node encode(const glm::mat3& rhs)
		{
			Node node;
			for (int i = 0; i < 3; i++)
				for (int j = 0; j < 3; j++)
					node.push_back(rhs[i][j]);
			return node;
		}

		static bool decode(const Node& node, glm::mat3& rhs)
		{
			if (!node.IsSequence() || node.size() != 9)
				return false;

			for (int i = 0; i < 3; i++)
				for (int j = 0; j < 3; j++)
					rhs[i][j] = node[i * 3 + j].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::mat4>
	{
		static Node encode(const glm::mat4& rhs)
		{
			Node node;
			for (int i = 0; i < 4; i++)
				for (int j = 0; j < 4; j++)
					node.push_back(rhs[i][j]);
			return node;
		}

		static bool decode(const Node& node, glm::mat4& rhs)
		{
			if (!node.IsSequence() || node.size() != 16)
				return false;

			for (int i = 0; i < 4; i++)
				for (int j = 0; j < 4; j++)
					rhs[i][j] = node[i * 4 + j].as<float>();
			return true;
		}
	};


	/*
	*	**********    COMPONENTS    **********
	*/

	template<>
	struct convert<Mule::TagComponent>
	{
		static Node encode(const Mule::TagComponent& rhs)
		{
			Node node;
			return node;
		}

		static bool decode(const Node& node, Mule::TagComponent& rhs)
		{
			//rhs.Tag = node["TAG_C"].as<std::wstring>();
			return true;
		}
	};

	template<>
	struct convert<Mule::TransformComponent>
	{
		static Node encode(const Mule::TransformComponent& rhs)
		{
			Node node;
			node["TRANSLATION"] = rhs.Translation;
			node["ORIENTATION"] = rhs.Orientation;
			node["SCALE"] = rhs.Scale;
			
			return node;
		}

		static bool decode(const Node& node, Mule::TransformComponent& rhs)
		{
			rhs.Translation = node["TRANSLATION"].as<glm::vec3>();
			rhs.Orientation = node["ORIENTATION"].as<glm::quat>();
			rhs.Scale = node["SCALE"].as<glm::vec3>();
			return true;
		}
	};

	template<>
	struct convert<Mule::GUIDComponent>
	{
		static Node encode(const Mule::GUIDComponent& rhs)
		{
			Node node;
			node["GUID"] = 0;// rhs.GUID;

			return node;
		}

		static bool decode(const Node& node, Mule::GUIDComponent& rhs)
		{
			rhs.GUID = Mule::GUID(node["GUID"].as<uint64_t>());
			return true;
		}
	};

	/*
	*	**********    ENTITY    **********
	*/

	template<>
	struct convert<Mule::Entity>
	{
		static Node encode(const Mule::Entity& rhs)
		{
			Node node;
			Node components = node[COMPONENTS];
			if (rhs.HasComponent<Mule::TagComponent>())
				components[TAG_COMPONENT] = rhs.GetComponent<Mule::TagComponent>();
			if (rhs.HasComponent<Mule::GUIDComponent>())
				components[GUID_COMPONENT] = rhs.GetComponent<Mule::GUIDComponent>();
			if (rhs.HasComponent<Mule::TransformComponent>())
				components[TRANSFORM_COMPONENT] = rhs.GetComponent<Mule::TransformComponent>();

			return node;
		}


	};
}