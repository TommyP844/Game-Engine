#pragma once

#include "GraphicsDevice.h"
#include "Util/Buffer.h"
#include "RenderPass.h"
#include "Asset/Asset.h"

#include "DiligentCore/Graphics/GraphicsTools/interface/MapHelper.hpp"
#include "UniformBuffer.h"
#include "Texture.h"

#include <filesystem>

namespace fs = std::filesystem;

namespace Mule
{
	struct SourceShaderDescription
	{
		RenderContext Context;
		WeakRef<GraphicsDevice> Device;
		Ref<RenderPass> RenderPass;

		bool EnableBlending;
		bool EnableDepthTest;
		uint32_t SubPassIndex;

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

		void Bind();
		void BindResources();
		void SetUniformBuffer(ShaderStage stage, const std::string& bufferName, Ref<UniformBuffer> buffer);
		void SetTexture(const std::string& name, Ref<Texture> texture);
		void SetArrayTexture(const std::string& name, Ref<Texture> texture, int index);

		template<typename T>
		void SetShaderVar(const std::string& name, ShaderStage stage, T& data)
		{
			auto buffer = mConstantBuffers[stage];
			Diligent::MapHelper<T> Constants(mContext.GetContext(), buffer, Diligent::MAP_WRITE, Diligent::MAP_FLAG_DISCARD);
			*Constants = data;
		}

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