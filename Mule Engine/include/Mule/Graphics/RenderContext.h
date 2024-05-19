#pragma once

#define PLATFORM_WIN32 1
#define VULKAN_SUPPORTED 1
#define DILIGENT_CPP_INTERFACE 1
#include "DiligentCore/Graphics/GraphicsEngine/interface/DeviceContext.h"

namespace Mule
{
	class Mesh;

	class RenderContext
	{
	public:
		RenderContext(){}
		RenderContext(Diligent::IDeviceContext* context, uint32_t preferredContext)
			:
			mContext(context),
			mPreferredContextMask(preferredContext)
		{}

		uint32_t GetContextMask() const { return mPreferredContextMask; }
		Diligent::IDeviceContext* GetContext() const { return mContext; }

	private:
		Diligent::IDeviceContext* mContext;
		uint32_t mPreferredContextMask;
	};
}
