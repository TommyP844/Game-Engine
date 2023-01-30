#pragma once

#include "Mule/Ref.h"
#include "Mule/Renderer/ShaderStage.h"


namespace Mule
{
	class Shader;

	struct UniformBufferDescription
	{
		UniformBufferDescription(int size, int binding, ShaderStage stage) : BufferSize(size), Stage(stage), Binding(binding) {}
		int BufferSize;
		int Binding;
		ShaderStage Stage;
	};

	class UniformBuffer
	{
	public:
		UniformBuffer(){}
		~UniformBuffer(){}

		static Ref<UniformBuffer> Create(const UniformBufferDescription& desc);

		
		virtual void Bind(Ref<Shader> shader);
		virtual void SetBufferData(void* data) {}
		virtual void GetBufferData(void* data) {}

		// Vulkan
		virtual VkDescriptorSetLayout GetDescriptorSetLayout() { return VkDescriptorSetLayout(); }


	};
}
