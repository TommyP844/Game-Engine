#pragma once

#include <glm.hpp>

namespace Mule
{
    struct Vertex
    {
        glm::vec3 Position;
        glm::vec2 TexCoord_0;
        glm::vec3 Normal;
        glm::vec3 Tangent;
        // Bi-Tangent can be calculated in shader
        glm::vec4 Color;
    };

	struct AnimatedVertex : Vertex
	{
        glm::vec4 BoneWeights;
        glm::ivec4 BoneIndices;
	};
}