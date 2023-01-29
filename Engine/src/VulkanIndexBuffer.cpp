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
		vk::Device device = RenderAPI::GetDevice();

		vk::DeviceSize bufferSize = sizeof(unsigned int) * length;

		vk::Buffer stagingBuffer;
		vk::DeviceMemory stagingBufferMemory;
		
		RenderAPI::CreateBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, indices, (size_t)bufferSize);
		vkUnmapMemory(device, stagingBufferMemory);

		Ref<VulkanIndexBuffer> ibuffer = MakeRef<VulkanIndexBuffer>();

		ibuffer->mNumIndices = length;

		RenderAPI::CreateBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, ibuffer->mBuffer, ibuffer->mBufferMemory);
		
		RenderAPI::CopyBuffer(stagingBuffer, ibuffer->mBuffer, bufferSize);

		vkDestroyBuffer(device, stagingBuffer, nullptr);
		vkFreeMemory(device, stagingBufferMemory, nullptr);

		return ibuffer;
	}

	void VulkanIndexBuffer::Bind()
	{
		vk::CommandBuffer commandbuffer = RenderAPI::GetActiveCommandBuffer();
		commandbuffer.bindIndexBuffer(mBuffer, 0, vk::IndexType::eUint32);
	}

	void VulkanIndexBuffer::Draw()
	{
		vk::CommandBuffer commandbuffer = RenderAPI::GetActiveCommandBuffer();
		commandbuffer.drawIndexed(mNumIndices, 1, 0, 0, 0);
	}

}