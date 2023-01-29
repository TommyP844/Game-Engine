#pragma once

#include "vulkan/vulkan.hpp"

#include "Mule/Ref.h"
#include "Mule/Renderer/VertexBuffer.h"

namespace Mule
{
	class VulkanVertexBuffer : public VertexBuffer
	{
	public:
		VulkanVertexBuffer(){}
		~VulkanVertexBuffer();
		static Ref<VulkanVertexBuffer> Create(void* vertices, int length, int vertexSize);
		virtual void Bind() override;
	private:
		vk::Buffer mBuffer;
		vk::DeviceMemory mDeviceMemory;
	};
}