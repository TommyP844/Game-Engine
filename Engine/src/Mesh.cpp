#include "mulepch.h"
#include "Mule/Renderer/Mesh.h"

namespace Mule
{
	Mesh::Mesh()
	{

	}

	Mesh::~Mesh()
	{
	}

	Ref<Mesh> Mesh::Create(Ref<VertexBuffer> vertexBuffer, Ref<IndexBuffer> indexBuffer)
	{
		Ref<Mesh> mesh = MakeRef<Mesh>();
		mesh->mVertexBuffer = vertexBuffer;
		mesh->mIndexBuffer = indexBuffer;
		return mesh;
	}

	void Mesh::Draw()
	{
		mVertexBuffer->Bind();
		mIndexBuffer->Bind();
		mIndexBuffer->Draw();
	}
}
