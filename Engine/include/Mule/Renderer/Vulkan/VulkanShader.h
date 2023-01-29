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
		virtual vk::PipelineLayout GetPipelineLayout() override;

	private:
		vk::Pipeline mPipeline;
		vk::PipelineLayout mPipelineLayout;	
		std::vector<vk::PushConstantRange> mPushConstants;
		void* mPushConstantData;

		static vk::PipelineShaderStageCreateInfo CreateShaderStageInfo(ShaderStage stage, vk::ShaderModule module);
		
		static vk::ShaderModule LoadModule(const std::vector<char>& code);
		
		static std::vector<char> LoadSource(const std::filesystem::path& filepath);

		struct VertexDescData
		{
			vk::VertexInputBindingDescription VertexBindingInfo;
			std::vector<vk::VertexInputAttributeDescription> Attribs;
		};

		static VertexDescData GetDescriptions(const VertexLayout& layout);

		static vk::Format GetFormatFromType(Type type);

		static vk::ShaderStageFlagBits GetShaderStage(ShaderStage stage);

		static std::vector<vk::PushConstantRange> GenPushConstants(const std::vector<ShaderAttachment>& attachments);
		
	};
}