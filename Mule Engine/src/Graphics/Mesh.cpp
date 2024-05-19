#include "Graphics/Mesh.h"

namespace Mule
{
	Ref<Mesh> Mesh::Create(WeakRef<GraphicsDevice> device, const MeshDescription& desc)
	{
		return Ref<Mesh>(new Mesh(device, desc));
	}

	void Mesh::Bind()
	{
		uint64_t offsets[] = { 0 };
		mContext.GetContext()->SetVertexBuffers(0, 1,
			&mVertexBuffer, offsets,
			Diligent::RESOURCE_STATE_TRANSITION_MODE_NONE);

		mContext.GetContext()->SetIndexBuffer(mIndexBuffer, 0, Diligent::RESOURCE_STATE_TRANSITION_MODE_NONE);
	}

	void Mesh::Draw(int instances)
	{
		Diligent::DrawIndexedAttribs attribs{};
		attribs.BaseVertex = 0;
		attribs.FirstIndexLocation = 0;
		attribs.FirstInstanceLocation = 0;
		attribs.IndexType = Diligent::VALUE_TYPE::VT_INT16;
		attribs.NumIndices = mNumIndices;
		attribs.NumInstances = instances;
		attribs.Flags = Diligent::DRAW_FLAGS::DRAW_FLAG_VERIFY_ALL;

		mContext.GetContext()->DrawIndexed(attribs);
	}

	Mesh::Mesh(WeakRef<GraphicsDevice> device, const MeshDescription& desc)
		:
		mContext(desc.Context)
	{
		auto diligentDevice = device->GetRenderDevice();

		Diligent::BufferDesc bufferDesc{};
		bufferDesc.Size = desc.Vertices.TypeSize * desc.Vertices.Count();
		bufferDesc.CPUAccessFlags = Diligent::CPU_ACCESS_NONE;
		bufferDesc.ElementByteStride = desc.VertexStride;
		bufferDesc.Usage = Diligent::USAGE::USAGE_IMMUTABLE;
		bufferDesc.Name = desc.Name.c_str();
		bufferDesc.BindFlags = Diligent::BIND_FLAGS::BIND_VERTEX_BUFFER;
		bufferDesc.Mode = Diligent::BUFFER_MODE_RAW;
		bufferDesc.MiscFlags = Diligent::MISC_BUFFER_FLAGS::MISC_BUFFER_FLAG_NONE;
		bufferDesc.ImmediateContextMask = desc.Context.GetContextMask();

		Diligent::BufferData data;
		data.DataSize = desc.Vertices.TypeSize * desc.Vertices.Count();
		data.pData = desc.Vertices.Data();
		data.pContext = desc.Context.GetContext();

		diligentDevice->CreateBuffer(bufferDesc, &data, &mVertexBuffer);


		Diligent::BufferDesc indexBufferDesc{};
		indexBufferDesc.Size = desc.Indices.TypeSize * desc.Indices.Count();
		indexBufferDesc.CPUAccessFlags = Diligent::CPU_ACCESS_NONE;
		indexBufferDesc.ElementByteStride = desc.VertexStride;
		indexBufferDesc.Usage = Diligent::USAGE::USAGE_IMMUTABLE;
		indexBufferDesc.Name = desc.Name.c_str();
		indexBufferDesc.BindFlags = Diligent::BIND_FLAGS::BIND_INDEX_BUFFER;
		indexBufferDesc.Mode = Diligent::BUFFER_MODE_RAW;
		indexBufferDesc.MiscFlags = Diligent::MISC_BUFFER_FLAGS::MISC_BUFFER_FLAG_NONE;
		indexBufferDesc.ImmediateContextMask = desc.Context.GetContextMask();

		mNumIndices = desc.Indices.Count();
		mNumTriangles = mNumIndices / 3;

		Diligent::BufferData indexBufferData;
		data.DataSize = desc.Indices.TypeSize * desc.Indices.Count();
		data.pData = desc.Indices.Data();
		data.pContext = desc.Context.GetContext();

		diligentDevice->CreateBuffer(indexBufferDesc, &indexBufferData, &mVertexBuffer);
	}
}