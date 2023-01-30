#include "mulepch.h"
#include "Mule/Renderer/Vulkan/VulkanIndexBuffer.h"
#include "Mule/Renderer/RenderAPI.h"
#include "Mule/Util/Log.h"

namespace Mule
{

	VulkanIndexBuffer::VulkanIndexBuffer()
	{
	}

	VulkanIndexBuffer::~VulkanIndexBuffer()
	{
	}

	Ref<VulkanIndexBuffer> VulkanIndexBuffer::Create(unsigned int* indices, int length)
	{
		VkDevice device = RenderAPI::GetDevice();

		VkDeviceSize bufferSize = sizeof(unsigned int) * length;

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		
		RenderAPI::CreateBuffer(bufferSize, VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
		
		void* data;
		vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, indices, (size_t)bufferSize);
		vkUnmapMemory(device, stagingBufferMemory);

		Ref<VulkanIndexBuffer> ibuffer = MakeRef<VulkanIndexBuffer>();

		ibuffer->mNumIndices = length;

		RenderAPI::CreateBuffer(bufferSize, VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, ibuffer->mBuffer, ibuffer->mBufferMemory);
		
		RenderAPI::CopyBuffer(stagingBuffer, ibuffer->mBuffer, bufferSize);

		vkDestroyBuffer(device, stagingBuffer, nullptr);
		vkFreeMemory(device, stagingBufferMemory, nullptr);

		return ibuffer;
	}

	void VulkanIndexBuffer::Bind()
	{
		VkCommandBuffer commandbuffer = RenderAPI::GetActiveCommandBuffer();
		vkCmdBindIndexBuffer(commandbuffer, mBuffer, 0, VK_INDEX_TYPE_UINT32);
	}

	void VulkanIndexBuffer::Draw()
	{
		VkCommandBuffer commandbuffer = RenderAPI::GetActiveCommandBuffer();
		vkCmdDrawIndexed(commandbuffer, mNumIndices, 1, 0, 0, 0);
	}

}