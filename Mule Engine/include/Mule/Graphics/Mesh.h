#pragma once

#include "GraphicsDevice.h"
#include "Util/Buffer.h"
#include "RenderContext.h"
#include "Asset/Asset.h"
#include <glm.hpp>

namespace Mule
{
	struct MeshDescription
	{
		std::string Name;
		Buffer<float> Vertices;
		Buffer<uint16_t> Indices;
		uint32_t VertexStride;
		RenderContext Context;
		glm::mat4 OffsetMatrix;
		AssetHandle Handle = Asset::GenerateHandle();
		AssetHandle DefaultMaterialHandle;
	};

	class Mesh : public Asset
	{
	public:
		static Ref<Mesh> Create(WeakRef<GraphicsDevice> device, const MeshDescription& desc);

		void Bind();

		void Draw(int instances = 0);

		const glm::mat4& GetOffsetMatrix() const { return mOffsetMatrix; }

		AssetHandle GetDefaultMaterialHandle() const { return mDefaultMaterialHandle; }

	private:
		Mesh(WeakRef<GraphicsDevice> device, const MeshDescription& desc);

		AssetHandle mDefaultMaterialHandle;
		glm::mat4 mOffsetMatrix;
		int mNumIndices;
		int mNumTriangles;
		RenderContext mContext;
		Diligent::RefCntAutoPtr<Diligent::IBuffer> mVertexBuffer;
		Diligent::RefCntAutoPtr<Diligent::IBuffer> mIndexBuffer;
	};
}
