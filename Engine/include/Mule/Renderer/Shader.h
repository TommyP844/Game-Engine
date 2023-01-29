#pragma once

#include "Mule/Ref.h"

#include "VertexLayout.h"
#include "ShaderStage.h"
#include "UniformBuffer.h"

namespace Mule
{
	struct ShaderAttachment
	{
		ShaderAttachment(ShaderStage stage, const std::filesystem::path& path, int pushConstantSize = 0)
			:
			Stage(stage),
			Path(path),
			PushConstantSize(pushConstantSize)
		{}
		ShaderStage Stage;
		std::filesystem::path Path;
		int PushConstantSize;
	};

	struct ShaderDescription
	{
		ShaderDescription(
			const std::initializer_list<ShaderAttachment>& attachments,
			const std::initializer_list<Ref<UniformBuffer>> buffers,
			const VertexLayout& layout = VertexLayout::Default())
			:
			Attachments(attachments),
			VertexLayout(layout),
			Buffers(buffers)
		{}

		std::vector<ShaderAttachment> Attachments;
		std::vector<Ref<UniformBuffer>> Buffers;
		VertexLayout VertexLayout;
	};

	class Shader
	{
	public:

		static Ref<Shader> Create(const ShaderDescription& shaderDesc);

		static void Compile(const std::filesystem::path& shader,
			const std::filesystem::path& output);

		virtual void BindBuffer(Ref<UniformBuffer> uniformBuffer){}
		virtual void Bind(){}
		virtual void UnBind(){}
		virtual void BindPushConstant(ShaderStage stage, void* data){}
		virtual vk::PipelineLayout GetPipelineLayout() { return vk::PipelineLayout(); }
	};
}
