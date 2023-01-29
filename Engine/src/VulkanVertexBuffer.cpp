#include "mulepch.h"
#include "Mule/Renderer/Vulkan/VulkanVertexBuffer.h"
#include "Mule/Renderer/RenderAPI.h"

namespace Mule
{
    Ref<VulkanVertexBuffer> VulkanVertexBuffer::Create(void* vertices, int length, int vertexSize)
    {
        vk::Device device = RenderAPI::GetDevice();

        vk::DeviceSize bufferSize = (uint64_t)vertexSize * (uint64_t)length;

        vk::Buffer stagingBuffer;
        vk::DeviceMemory stagingBufferMemory;
        RenderAPI::CreateBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory);
        
        vk::DeviceSize offset = 0;
        void* data = device.mapMemory(stagingBufferMemory, 0, bufferSize);
        memcpy(data, vertices, (size_t)bufferSize);
        device.unmapMemory(stagingBufferMemory);

        Ref<VulkanVertexBuffer> vbuffer = MakeRef<VulkanVertexBuffer>();
        RenderAPI::CreateBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, vbuffer->mBuffer, vbuffer->mDeviceMemory);
        
        RenderAPI::CopyBuffer(stagingBuffer, vbuffer->mBuffer, bufferSize);

        device.destroyBuffer(stagingBuffer);
        device.freeMemory(stagingBufferMemory);

        return vbuffer;
    }

    VulkanVertexBuffer::~VulkanVertexBuffer()
    {
    }

    void VulkanVertexBuffer::Bind()
    {
        vk::CommandBuffer commandBuffer = RenderAPI::GetActiveCommandBuffer();
        vk::DeviceSize offsets[] = { 0 };
        commandBuffer.bindVertexBuffers(0, mBuffer, offsets);
    }
}