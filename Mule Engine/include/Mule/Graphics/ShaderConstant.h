#pragma once

#include <Ref.h>
#include "GraphicsDevice.h"

namespace Mule
{
	class ShaderConstant
	{
	public:
		static Ref<ShaderConstant> Create(WeakRef<GraphicsDevice> device, RenderContext context, const std::string& name, uint32_t size);

		void SetData(void* data, int size, int offset = 0);
		
		template<typename T>
		void SetData(const T& data, uint32_t offset = 0)
		{
			mContext.GetContext()->UpdateBuffer(mBuffer, offset, sizeof(T), &data, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
		}

		const std::string& GetName() const { return mName; }

		Diligent::RefCntAutoPtr<Diligent::IBuffer> GetBuffer() const { return mBuffer; }

	private:
		ShaderConstant(WeakRef<GraphicsDevice> device, RenderContext context, const std::string& name, uint32_t size);

		RenderContext mContext;
		uint32_t mSize;
		std::string mName;
		Diligent::RefCntAutoPtr<Diligent::IBuffer> mBuffer;
	};
}