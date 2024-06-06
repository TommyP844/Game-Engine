#include "Graphics/UniformBuffer.h"

namespace Mule
{
	Ref<UniformBuffer> UniformBuffer::Create(WeakRef<GraphicsDevice> device, const UniformBufferDescription& desc)
	{
		return Ref<UniformBuffer>(new UniformBuffer(device, desc));
	}
	
	void UniformBuffer::SetData(RenderContext context, void* data, uint32_t size, uint32_t offset)
	{
		context.GetContext()->UpdateBuffer(mUniformBuffer, offset, size, data, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
	}

	UniformBuffer::UniformBuffer(WeakRef<GraphicsDevice> device, const UniformBufferDescription& desc)
	{
		auto diligentDevice = device->GetRenderDevice();

		Diligent::BufferDesc bufferDesc;
		bufferDesc.Name = "";
		bufferDesc.Usage = Diligent::USAGE_DYNAMIC;
		bufferDesc.CPUAccessFlags = Diligent::CPU_ACCESS_WRITE;
		bufferDesc.Size = desc.Size;
		bufferDesc.BindFlags = Diligent::BIND_FLAGS::BIND_UNIFORM_BUFFER;
		bufferDesc.Usage = Diligent::USAGE_DYNAMIC;
		
		diligentDevice->CreateBuffer(bufferDesc, nullptr, &mUniformBuffer);

		
	}
}