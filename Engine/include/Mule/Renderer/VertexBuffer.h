#pragma once

#include "Mule/Ref.h"

namespace Mule
{
	class VertexBuffer
	{
	public:
		static Ref<VertexBuffer> Create(void* vertices, int length, int vertexSize);

		virtual void Bind(){}
		virtual void UnBind(){}

	};
}