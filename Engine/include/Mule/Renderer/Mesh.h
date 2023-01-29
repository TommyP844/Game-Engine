#pragma once

#include "Mule/Ref.h"
#include "Mule/Renderer/VertexBuffer.h"
#include "Mule/Renderer/IndexBuffer.h"
#include "Mule/Renderer/VertexDefault.h"
#include <glm/glm.hpp>

namespace Mule
{

	class Mesh
	{
	public:
		Mesh();
		~Mesh();		

		static Ref<Mesh> Create(Ref<VertexBuffer> vertexBuffer, Ref<IndexBuffer> indexBuffer);

		void Draw();

	private:
		Ref<VertexBuffer> mVertexBuffer;
		Ref<IndexBuffer> mIndexBuffer;

	};
}