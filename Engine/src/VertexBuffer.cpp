#include "mulepch.h"
#include "Mule/Renderer/VertexBuffer.h"
#include "Mule/Renderer/Vulkan/VulkanVertexBuffer.h"
#include "Mule/Renderer/RenderAPI.h"

namespace Mule
{

    Ref<VertexBuffer> VertexBuffer::Create(void* vertices, int length, int vertexSize)
    {
        switch (RenderAPI::GetAPI())
        {
        case Mule::RenderAPI::API::VULKAN:
            return VulkanVertexBuffer::Create(vertices, length, vertexSize);
            break;
        }
    }

}
