#include "Graphics/UniformBuffer.h"

namespace Mule
{
	Ref<UniformBuffer> UniformBuffer::Create(WeakRef<GraphicsDevice> device, const UniformBufferDescription& desc)
	{
		return Ref<UniformBuffer>(new UniformBuffer(device, desc));
	}

	void UniformBuffer::SetData(void* data, uint32_t size, uint32_t offset)
	{
		// TODO:
		mContext->UpdateBuffer(mUniformBuffer, offset, size, data, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
	}

	void UniformBuffer::ReadData(void* data, uint32_t size, uint32_t offset)
	{
		// TODO:
		return;
	}

	UniformBuffer::UniformBuffer(WeakRef<GraphicsDevice> device, const UniformBufferDescription& desc)
	{
		auto diligentDevice = device->GetRenderDevice();

		Diligent::BufferDesc bufferDesc{};
		bufferDesc.Name = desc.Name.c_str();
		bufferDesc.BindFlags = Diligent::BIND_FLAGS::BIND_SHADER_RESOURCE;
		bufferDesc.ElementByteStride = desc.Stride;
		bufferDesc.Size = desc.Size;

		Diligent::USAGE usage = Diligent::USAGE::USAGE_IMMUTABLE;
		if (desc.IsBufferDynamic)
			usage = Diligent::USAGE::USAGE_DYNAMIC;

		bufferDesc.Usage = usage;
		bufferDesc.MiscFlags = Diligent::MISC_BUFFER_FLAGS::MISC_BUFFER_FLAG_NONE;
		bufferDesc.Mode = Diligent::BUFFER_MODE::BUFFER_MODE_STRUCTURED;
		bufferDesc.ImmediateContextMask = 0;
		bufferDesc.CPUAccessFlags = Diligent::CPU_ACCESS_NONE;

		Diligent::BufferData data{};
		data.DataSize = desc.Size;
		data.pContext = nullptr;
		data.pData = desc.Data;

		diligentDevice->CreateBuffer(bufferDesc, &data, &mUniformBuffer);
	}
}