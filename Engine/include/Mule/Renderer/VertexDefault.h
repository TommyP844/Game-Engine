#pragma once

#include <glm/glm.hpp>

namespace Mule
{
	struct Vertex
	{
		Vertex() = default;
		Vertex(const Vertex&) = default;
		Vertex& operator=(const Vertex& other) = default;
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec3 Tangent;
		glm::vec3 BiTangent;
		glm::vec3 Color;
		glm::vec2 UV;
	};
}