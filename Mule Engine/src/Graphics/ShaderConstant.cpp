#include "Graphics/ShaderConstant.h"

namespace Mule
{
	Ref<ShaderConstant> ShaderConstant::Create(WeakRef<GraphicsDevice> device, RenderContext context, const std::string& name, uint32_t size)
	{
		return Ref<ShaderConstant>(new ShaderConstant(device, context, name, size));
	}

	ShaderConstant::ShaderConstant(WeakRef<GraphicsDevice> device, RenderContext context, const std::string& name, uint32_t size)
		:
		mContext(context),
		mName(name),
		mSize(size)
	{
		auto diligentDevice = device->GetRenderDevice();

		Diligent::BufferDesc bufferDesc;
		bufferDesc.Name = name.c_str();
		bufferDesc.Usage = Diligent::USAGE_DYNAMIC;
		bufferDesc.CPUAccessFlags = Diligent::CPU_ACCESS_WRITE;
		bufferDesc.Size = size;
		bufferDesc.BindFlags = Diligent::BIND_FLAGS::BIND_SHADER_RESOURCE;

		diligentDevice->CreateBuffer(bufferDesc, nullptr, &mBuffer);		
	}

	void ShaderConstant::SetData(void* data, int size, int offset)
	{
		mContext.GetContext()->UpdateBuffer(mBuffer, offset, size, data, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
	}
	
}