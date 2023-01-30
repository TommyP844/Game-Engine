#include "mulepch.h"
#include "Mule/Renderer/Vulkan/VulkanVertexBuffer.h"
#include "Mule/Renderer/RenderAPI.h"

namespace Mule
{
    Ref<VulkanVertexBuffer> VulkanVertexBuffer::Create(void* vertices, int length, int vertexSize)
    {
        VkDevice device = RenderAPI::GetDevice();

        VkDeviceSize bufferSize = (uint64_t)vertexSize * (uint64_t)length;

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        RenderAPI::CreateBuffer(bufferSize, VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
        
        void* data = NULL;
        vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices, (size_t)bufferSize);
        vkUnmapMemory(device, stagingBufferMemory);

        Ref<VulkanVertexBuffer> vbuffer = MakeRef<VulkanVertexBuffer>();
        RenderAPI::CreateBuffer(bufferSize, VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vbuffer->mBuffer, vbuffer->mDeviceMemory);
        
        RenderAPI::CopyBuffer(stagingBuffer, vbuffer->mBuffer, bufferSize);

        vkDestroyBuffer(device, stagingBuffer, nullptr);
        vkFreeMemory(device, stagingBufferMemory, nullptr);

        return vbuffer;
    }

    VulkanVertexBuffer::~VulkanVertexBuffer()
    {
    }

    void VulkanVertexBuffer::Bind()
    {
        VkCommandBuffer commandBuffer = RenderAPI::GetActiveCommandBuffer();
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, &mBuffer, offsets);
    }
}