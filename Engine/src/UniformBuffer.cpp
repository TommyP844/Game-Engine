#include "mulepch.h"
#include "Mule/Renderer/UniformBuffer.h"

#include "Mule/Renderer/RenderAPI.h"

#include "Mule/Renderer/Vulkan/VulkanUniformBuffer.h"
#include "Mule/Renderer/Shader.h"

namespace Mule
{
	Ref<UniformBuffer> UniformBuffer::Create(const UniformBufferDescription& desc)
	{
		switch (RenderAPI::GetAPI())
		{
		case RenderAPI::API::VULKAN:
			return VulkanUniformBuffer::Create(desc);
			break;
		}
		return Ref<UniformBuffer>();
	}
	void UniformBuffer::Bind(Ref<Shader> shader)
	{
	}
}