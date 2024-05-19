#pragma once

#include "GraphicsDevice.h"
#include "Util/Buffer.h"
#include "RenderContext.h"

namespace Mule
{
	struct MeshDescription
	{
		std::string Name;
		Buffer<float> Vertices;
		Buffer<uint16_t> Indices;
		uint32_t VertexStride;
		RenderContext Context;
	};

	class Mesh
	{
	public:
		static Ref<Mesh> Create(WeakRef<GraphicsDevice> device, const MeshDescription& desc);

		void Bind();

		void Draw(int instances = 0);

	private:
		Mesh(WeakRef<GraphicsDevice> device, const MeshDescription& desc);

		int mNumIndices;
		int mNumTriangles;
		RenderContext mContext;
		Diligent::RefCntAutoPtr<Diligent::IBuffer> mVertexBuffer;
		Diligent::RefCntAutoPtr<Diligent::IBuffer> mIndexBuffer;
	};
}
