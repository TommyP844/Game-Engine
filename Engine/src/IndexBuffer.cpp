#include "mulepch.h"
#include "Mule/Renderer/IndexBuffer.h"
#include "Mule/Renderer/RenderAPI.h"

#include "Mule/Renderer/Vulkan/VulkanIndexBuffer.h"

namespace Mule
{
	Ref<IndexBuffer> IndexBuffer::Create(unsigned int* indices, int count)
	{
		switch (RenderAPI::GetAPI())
		{
		case Mule::RenderAPI::API::VULKAN:
			return VulkanIndexBuffer::Create(indices, count);
			break;
		}
		return Ref<IndexBuffer>();
	}
}