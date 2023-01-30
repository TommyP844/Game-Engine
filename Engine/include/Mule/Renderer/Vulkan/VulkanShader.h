#pragma once

#include "Mule/Ref.h"
#include "Mule/Renderer/Shader.h"
#include "Mule/Renderer/VertexLayout.h"
#include "Mule/Renderer/ShaderStage.h"

#include <filesystem>
#include <string>
#include <tuple>

#include <vulkan/vulkan.hpp>

namespace Mule
{
	class VulkanShader : public Shader
	{
	public:
		VulkanShader();
		~VulkanShader();
		static Ref<VulkanShader> Create(const ShaderDescription& shaderDesc);

		virtual void BindPushConstant(ShaderStage stage, void* data) override;
		virtual void Bind() override;
		virtual void BindBuffer(Ref<UniformBuffer> uniformBuffer) override;
		virtual VkPipelineLayout GetPipelineLayout() override;

	private:
		VkPipeline mPipeline;
		VkPipelineLayout mPipelineLayout;	
		std::vector<VkPushConstantRange> mPushConstants;
		void* mPushConstantData;

		static VkPipelineShaderStageCreateInfo CreateShaderStageInfo(ShaderStage stage, VkShaderModule module);
		
		static VkShaderModule LoadModule(const std::vector<char>& code);
		
		static std::vector<char> LoadSource(const std::filesystem::path& filepath);

		struct VertexDescData
		{
			VkVertexInputBindingDescription VertexBindingInfo;
			std::vector<VkVertexInputAttributeDescription> Attribs;
		};

		static VertexDescData GetDescriptions(const VertexLayout& layout);

		static VkFormat GetFormatFromType(Type type);

		static VkShaderStageFlagBits GetShaderStage(ShaderStage stage);

		static std::vector<VkPushConstantRange> GenPushConstants(const std::vector<ShaderAttachment>& attachments);
		
	};
}