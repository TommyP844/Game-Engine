#include "mulepch.h"
#include "Mule/Renderer/Vulkan/VulkanUniformBuffer.h"

#include "Mule/Renderer/RenderAPI.h"

namespace Mule
{
	VulkanUniformBuffer::~VulkanUniformBuffer()
	{
		vk::Device device = RenderAPI::GetDevice();

		for (auto& buffer : mUniformBuffer)
		{
			device.destroyBuffer(buffer);
		}

		for (auto& deviceMemory : mUniformBufferMemory)
		{
			device.unmapMemory(deviceMemory);
			device.freeMemory(deviceMemory);
		}
	}

	Ref<VulkanUniformBuffer> VulkanUniformBuffer::Create(const UniformBufferDescription& desc)
	{
		Ref<VulkanUniformBuffer> uniformBuffer = MakeRef<VulkanUniformBuffer>();

		const int BUFFER_COUNT = RenderAPI::GetSwapChainBufferCount();
		const vk::Device device = RenderAPI::GetDevice();

		VkDeviceSize bufferSize = desc.BufferSize;

		uniformBuffer->mBuffersize = bufferSize;
		uniformBuffer->mUniformBuffer.resize(BUFFER_COUNT);
		uniformBuffer->mUniformBufferMemory.resize(BUFFER_COUNT);
		uniformBuffer->mUniformBufferPtr.resize(BUFFER_COUNT);

		for (size_t i = 0; i < BUFFER_COUNT; i++) {
			RenderAPI::CreateBuffer( 
				bufferSize, 
				vk::BufferUsageFlagBits::eUniformBuffer, 
				vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
				uniformBuffer->mUniformBuffer[i],
				uniformBuffer->mUniformBufferMemory[i]);

			vkMapMemory(device, uniformBuffer->mUniformBufferMemory[i], 0, bufferSize, 0, &uniformBuffer->mUniformBufferPtr[i]);
		}

		vk::DescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = desc.Binding;
		uboLayoutBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = GetShaderStage(desc.Stage);

		vk::DescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = vk::StructureType::eDescriptorSetLayoutCreateInfo;
		layoutInfo.bindingCount = 1;
		layoutInfo.pBindings = &uboLayoutBinding;

		vk::DescriptorSetLayout descriptorSetLayout = device.createDescriptorSetLayout(layoutInfo);

		uniformBuffer->mDescriptorSetLayouts = std::vector<vk::DescriptorSetLayout>(BUFFER_COUNT, descriptorSetLayout);
		
		vk::DescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = vk::StructureType::eDescriptorSetAllocateInfo;
		allocInfo.descriptorPool = RenderAPI::GetDescriptorPool();
		allocInfo.descriptorSetCount = static_cast<uint32_t>(BUFFER_COUNT);
		allocInfo.pSetLayouts = &uniformBuffer->mDescriptorSetLayouts[0];

		uniformBuffer->mDescriptorSets = device.allocateDescriptorSets(allocInfo);

		for (size_t i = 0; i < BUFFER_COUNT; i++) {
			vk::DescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = uniformBuffer->mUniformBuffer[i];
			bufferInfo.offset = 0;
			bufferInfo.range = desc.BufferSize;

			// images
			// VkDescriptorImageInfo imageInfo{};
			// imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			// imageInfo.imageView = textureImageView;
			// imageInfo.sampler = textureSampler;

			vk::WriteDescriptorSet descriptorWrites{};

			descriptorWrites.sType = vk::StructureType::eWriteDescriptorSet;
			descriptorWrites.dstSet = uniformBuffer->mDescriptorSets[i];
			descriptorWrites.dstBinding = 0;
			descriptorWrites.dstArrayElement = 0;
			descriptorWrites.descriptorType = vk::DescriptorType::eUniformBuffer;
			descriptorWrites.descriptorCount = 1;
			descriptorWrites.pBufferInfo = &bufferInfo;

			// descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			// descriptorWrites[1].dstSet = uniformBuffer->mDescriptorSets[i];
			// descriptorWrites[1].dstBinding = 1;
			// descriptorWrites[1].dstArrayElement = 0;
			// descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			// descriptorWrites[1].descriptorCount = 1;
			// descriptorWrites[1].pImageInfo = &imageInfo;

			device.updateDescriptorSets(descriptorWrites, {});
		}

		return uniformBuffer;
	}

	void VulkanUniformBuffer::Bind(Ref<Shader> shader)
	{
		auto cmd = RenderAPI::GetActiveCommandBuffer();
		int currentFrame = RenderAPI::GetCurrentSwapChainFrame();
		cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, shader->GetPipelineLayout(), 0, mDescriptorSets[currentFrame], {});
		
	}
	
	void VulkanUniformBuffer::GetBufferData(void* data)
	{
		memcpy(data, mUniformBufferPtr[RenderAPI::GetCurrentSwapChainFrame()], mBuffersize);
	}

	void VulkanUniformBuffer::SetBufferData(void* data)
	{
		memcpy(mUniformBufferPtr[RenderAPI::GetCurrentSwapChainFrame()], data, mBuffersize);
	}


	vk::DescriptorSetLayout VulkanUniformBuffer::GetDescriptorSetLayout()
	{
		return mDescriptorSetLayouts[RenderAPI::GetCurrentSwapChainFrame()];
	}
	
	vk::ShaderStageFlagBits VulkanUniformBuffer::GetShaderStage(ShaderStage stage)
	{
		switch (stage)
		{
		case ShaderStage::VERTEX: return vk::ShaderStageFlagBits::eVertex; break;
		case ShaderStage::FRAGMENT: return vk::ShaderStageFlagBits::eFragment; break;
		case ShaderStage::GEOMETRY: return vk::ShaderStageFlagBits::eGeometry; break;
		case ShaderStage::TESSELATION_CONTROL: return vk::ShaderStageFlagBits::eTessellationControl; break;
		case ShaderStage::TESSELATION_EVALUATION: return vk::ShaderStageFlagBits::eTessellationEvaluation; break;
		}
		return vk::ShaderStageFlagBits();
	}
}