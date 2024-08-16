#pragma once

#include "GraphicsDevice.h"
#include "Util/Buffer.h"
#include "RenderPass.h"
#include "Asset/Asset.h"

#include "DiligentCore/Graphics/GraphicsTools/interface/MapHelper.hpp"
#include "UniformBuffer.h"
#include "Texture.h"
#include "ShaderConstant.h"

#include <filesystem>

namespace fs = std::filesystem;

namespace Mule
{

	enum VertexAttribType
	{
		FLOAT = Diligent::VALUE_TYPE::VT_FLOAT32,
		INT = Diligent::VALUE_TYPE::VT_INT32
	};

	struct VertexAttrib
	{
		VertexAttrib(VertexAttribType type, int count) : Type(type), Count(count) {}
		VertexAttribType Type;
		int Count;
	};

	struct VertexLayout
	{
		bool PerInstance = false;
		int BufferIndex;
		std::vector<VertexAttrib> Attribs;
	};

	struct SourceShaderDescription
	{
		RenderContext Context;
		WeakRef<GraphicsDevice> Device;
		Ref<RenderPass> RenderPass;

		bool EnableBlending;
		bool EnableDepthTest;
		uint32_t SubPassIndex;

		std::vector<VertexLayout> VertexLayouts;

		std::string Name;
		fs::path Path;
	};

	enum class ShaderStage
	{
		Vertex = Diligent::SHADER_TYPE_VERTEX,
		Fragment = Diligent::SHADER_TYPE_PIXEL
	};

	class Shader : public Asset
	{
	public:
		static Ref<Shader> Create(const SourceShaderDescription& desc);

		static VertexLayout GetDefaultVertexLayout();

		void Bind();
		void BindResources();
		void SetUniformBuffer(ShaderStage stage, const std::string& bufferName, Ref<UniformBuffer> buffer);
		void SetTexture(const std::string& name, Ref<Texture> texture);
		void SetArrayTexture(const std::string& name, Ref<Texture> texture, int index);
		void SetShaderConstants(ShaderStage stage, Ref<ShaderConstant> buffer);

		void Reload();

	private:
		Shader(const SourceShaderDescription& desc);

		WeakRef<GraphicsDevice> mDevice;
		RenderContext mContext;

		Diligent::RefCntAutoPtr<Diligent::IShader> CreateShaderFromSource(const std::string& source, Diligent::SHADER_TYPE shaderType);

		Diligent::RefCntAutoPtr<Diligent::IShader> mVertexShader;
		Diligent::RefCntAutoPtr<Diligent::IShader> mFragmentShader;

		Diligent::RefCntAutoPtr<Diligent::IPipelineState> mPipeline;
		Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> mSRB;

		std::map<ShaderStage, Diligent::RefCntAutoPtr<Diligent::IBuffer>> mConstantBuffers;
	};
}