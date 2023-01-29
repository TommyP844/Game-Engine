#pragma once

#include "Mule/Ref.h"

namespace Mule
{
	class IndexBuffer
	{
	public:
		static Ref<IndexBuffer> Create(unsigned int* indices, int count);

		virtual void Draw() {}
		virtual void Bind() {}
		virtual void UnBind() {}
	};
}
