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

		virtual VkDescriptorSetLayout GetDescriptorSetLayout() override;

	private:
		std::vector<VkBuffer> mUniformBuffer;
		std::vector<VkDeviceMemory> mUniformBufferMemory;
		std::vector<void*> mUniformBufferPtr;
		std::vector<VkDescriptorSetLayout> mDescriptorSetLayouts;
		std::vector<VkDescriptorSet> mDescriptorSets;
		int mBuffersize;

		static VkShaderStageFlagBits GetShaderStage(ShaderStage stage);

	};
}