#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include <string>

#include "GUID.h"

namespace Mule {

	struct TransformComponent
	{
		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		glm::vec3 Translation;
		glm::quat Orientation;
		glm::vec3 Scale;
	};

	struct TagComponent
	{
		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::wstring& tag) : Tag(tag) {}
		std::wstring Tag;
	};

	struct GUIDComponent
	{
		GUIDComponent() = default;
		GUIDComponent(const GUIDComponent&) = default;
		GUIDComponent(GUID guid) : GUID(guid) {}
		GUID GUID;
	};

}
