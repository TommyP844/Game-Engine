#pragma once

#include <vulkan/vulkan.hpp>

#include "Mule/Ref.h"
#include "Mule/Renderer/IndexBuffer.h"

namespace Mule
{
	class VulkanIndexBuffer : public IndexBuffer
	{
	public:
		VulkanIndexBuffer();
		~VulkanIndexBuffer();
		static Ref<VulkanIndexBuffer> Create(unsigned int* indices, int length);

		virtual void Bind() override;
		virtual void Draw() override;

	private:
		int mNumIndices;
		VkBuffer mBuffer;
		VkDeviceMemory mBufferMemory;
	};
}
