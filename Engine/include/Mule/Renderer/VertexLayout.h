#pragma once

#include "Mule/Types.h"

#include <initializer_list>
#include <vector>

namespace Mule
{

	struct VertexElement
	{
		VertexElement(Type t, int c) : type(t), count(c) {}
		VertexElement(Type t) : type(t), count(1) {}
		Type type;
		int count;
	};

	struct VertexLayout
	{
		VertexLayout(const std::initializer_list<VertexElement>& list)
			:
			Elements(list)
		{}

		static VertexLayout Default()
		{
			return {
				{ Type::VEC3_F32 }, // Position
				{ Type::VEC3_F32 }, // Normal
				{ Type::VEC3_F32 }, // Tangent
				{ Type::VEC3_F32 }, // BiTangent
				{ Type::VEC3_F32 }, // Color
				{ Type::VEC2_F32 } // Color
			};
		}

		std::vector<VertexElement> Elements;
	};

}