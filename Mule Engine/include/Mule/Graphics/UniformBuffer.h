#pragma once

#include "GraphicsDevice.h"
#include "GraphicsTypes.h"

namespace Mule
{
	struct UniformBufferDescription
	{
		std::string Name;
		uint32_t Size;
		uint32_t Stride;
		void* Data;
		BufferAccess AccessMode;
		bool IsBufferDynamic;
	};

	class UniformBuffer
	{
	public:
		static Ref<UniformBuffer> Create(WeakRef<GraphicsDevice> device, const UniformBufferDescription& desc);

		void SetData(RenderContext context, void* data, uint32_t size, uint32_t offset = 0);

		Diligent::RefCntAutoPtr<Diligent::IBuffer> GetBuffer() const { return mUniformBuffer; }

	private:
		UniformBuffer(WeakRef<GraphicsDevice> device, const UniformBufferDescription& desc);

		Diligent::IDeviceContext* mContext;
		Diligent::RefCntAutoPtr<Diligent::IBuffer> mUniformBuffer;
	};
}