#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

struct TransformComponent
{
	glm::vec3 Translation;
	glm::quat Orientation;
	glm::vec3 Scale;
};
