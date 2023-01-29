#pragma once

#include "Mule/Renderer/UniformBuffer.h"
#include "Mule/Renderer/Shader.h"

#include <vulkan/vulkan.hpp>

#include <vector>

namespace Mule
{
	class VulkanUniformBuffer : public UniformBuffer
	{
	public:
		VulkanUniformBuffer(){}
		~VulkanUniformBuffer();

		static Ref<VulkanUniformBuffer> Create(const UniformBufferDescription& desc);

		virtual void Bind(Ref<Shader> shader) override;
		
		virtual void GetBufferData(void* data) override;
		
		virtual void SetBufferData(void* data) override;

		virtual vk::DescriptorSetLayout GetDescriptorSetLayout() override;

	private:
		std::vector<vk::Buffer> mUniformBuffer;
		std::vector<vk::DeviceMemory> mUniformBufferMemory;
		std::vector<void*> mUniformBufferPtr;
		std::vector<vk::DescriptorSetLayout> mDescriptorSetLayouts;
		std::vector<vk::DescriptorSet> mDescriptorSets;
		int mBuffersize;

		static vk::ShaderStageFlagBits GetShaderStage(ShaderStage stage);

	};
}