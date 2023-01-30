#include "mulepch.h"
#include "Mule/Renderer/Vulkan/VulkanUniformBuffer.h"

#include "Mule/Renderer/RenderAPI.h"

namespace Mule
{
	VulkanUniformBuffer::~VulkanUniformBuffer()
	{
		VkDevice device = RenderAPI::GetDevice();

		for (auto& buffer : mUniformBuffer)
		{
			vkDestroyBuffer(device, buffer, nullptr);
		}

		for (auto& deviceMemory : mUniformBufferMemory)
		{
			vkUnmapMemory(device, deviceMemory);
			vkFreeMemory(device, deviceMemory, nullptr);
		}
	}

	Ref<VulkanUniformBuffer> VulkanUniformBuffer::Create(const UniformBufferDescription& desc)
	{
		Ref<VulkanUniformBuffer> uniformBuffer = MakeRef<VulkanUniformBuffer>();

		const int BUFFER_COUNT = RenderAPI::GetSwapChainBufferCount();
		const VkDevice device = RenderAPI::GetDevice();

		VkDeviceSize bufferSize = desc.BufferSize;

		uniformBuffer->mBuffersize = bufferSize;
		uniformBuffer->mUniformBuffer.resize(BUFFER_COUNT);
		uniformBuffer->mUniformBufferMemory.resize(BUFFER_COUNT);
		uniformBuffer->mUniformBufferPtr.resize(BUFFER_COUNT);

		for (size_t i = 0; i < BUFFER_COUNT; i++) {
			RenderAPI::CreateBuffer( 
				bufferSize, 
				VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
				VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				uniformBuffer->mUniformBuffer[i],
				uniformBuffer->mUniformBufferMemory[i]);

			vkMapMemory(device, uniformBuffer->mUniformBufferMemory[i], 0, bufferSize, 0, &uniformBuffer->mUniformBufferPtr[i]);
		}

		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = desc.Binding;
		uboLayoutBinding.descriptorType = VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = GetShaderStage(desc.Stage);

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 1;
		layoutInfo.pBindings = &uboLayoutBinding;

		VkDescriptorSetLayout descriptorSetLayout;
		vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout);

		uniformBuffer->mDescriptorSetLayouts = std::vector<VkDescriptorSetLayout>(BUFFER_COUNT, descriptorSetLayout);
		
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = RenderAPI::GetDescriptorPool();
		allocInfo.descriptorSetCount = static_cast<uint32_t>(BUFFER_COUNT);
		allocInfo.pSetLayouts = &uniformBuffer->mDescriptorSetLayouts[0];

		uniformBuffer->mDescriptorSets.resize(BUFFER_COUNT);
		vkAllocateDescriptorSets(device, &allocInfo, &uniformBuffer->mDescriptorSets[0]);

		for (size_t i = 0; i < BUFFER_COUNT; i++) {
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = uniformBuffer->mUniformBuffer[i];
			bufferInfo.offset = 0;
			bufferInfo.range = desc.BufferSize;

			// images
			// VkDescriptorImageInfo imageInfo{};
			// imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			// imageInfo.imageView = textureImageView;
			// imageInfo.sampler = textureSampler;

			VkWriteDescriptorSet descriptorWrites{};

			descriptorWrites.sType = VkStructureType::VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites.dstSet = uniformBuffer->mDescriptorSets[i];
			descriptorWrites.dstBinding = 0;
			descriptorWrites.dstArrayElement = 0;
			descriptorWrites.descriptorType = VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites.descriptorCount = 1;
			descriptorWrites.pBufferInfo = &bufferInfo;

			// descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			// descriptorWrites[1].dstSet = uniformBuffer->mDescriptorSets[i];
			// descriptorWrites[1].dstBinding = 1;
			// descriptorWrites[1].dstArrayElement = 0;
			// descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			// descriptorWrites[1].descriptorCount = 1;
			// descriptorWrites[1].pImageInfo = &imageInfo;

			vkUpdateDescriptorSets(device, 1, &descriptorWrites, 0, NULL);
		}

		return uniformBuffer;
	}

	void VulkanUniformBuffer::Bind(Ref<Shader> shader)
	{
		auto cmd = RenderAPI::GetActiveCommandBuffer();
		int currentFrame = RenderAPI::GetCurrentSwapChainFrame();
		vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, shader->GetPipelineLayout(), 0, 1, &mDescriptorSets[currentFrame], 0, NULL);
	}
	
	void VulkanUniformBuffer::GetBufferData(void* data)
	{
		memcpy(data, mUniformBufferPtr[RenderAPI::GetCurrentSwapChainFrame()], mBuffersize);
	}

	void VulkanUniformBuffer::SetBufferData(void* data)
	{
		memcpy(mUniformBufferPtr[RenderAPI::GetCurrentSwapChainFrame()], data, mBuffersize);
	}


	VkDescriptorSetLayout VulkanUniformBuffer::GetDescriptorSetLayout()
	{
		return mDescriptorSetLayouts[RenderAPI::GetCurrentSwapChainFrame()];
	}
	
	VkShaderStageFlagBits VulkanUniformBuffer::GetShaderStage(ShaderStage stage)
	{
		switch (stage)
		{
		case ShaderStage::VERTEX: return VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT; break;
		case ShaderStage::FRAGMENT: return VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT; break;
		case ShaderStage::GEOMETRY: return VkShaderStageFlagBits::VK_SHADER_STAGE_GEOMETRY_BIT; break;
		case ShaderStage::TESSELATION_CONTROL: return VkShaderStageFlagBits::VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT; break;
		case ShaderStage::TESSELATION_EVALUATION: return VkShaderStageFlagBits::VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT; break;
		}
		return VkShaderStageFlagBits();
	}
}