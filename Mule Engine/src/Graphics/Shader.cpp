#include "Graphics/Shader.h"

#include "Log.h"

namespace Mule
{
	Ref<Shader> Shader::Create(const SourceShaderDescription& desc)
	{
		return Ref<Shader>(new Shader(desc));
	}

	Ref<Shader> Shader::Create(const BinaryShaderDescription& desc)
	{
		return Ref<Shader>(new Shader(desc));
	}

	void Shader::Reload()
	{
	}

	Shader::Shader(const BinaryShaderDescription& desc)
	{
	}

	Diligent::RefCntAutoPtr<Diligent::IShader> Shader::CreateShaderFromSource(const fs::path& path)
	{
		auto diligentDevice = mDevice->GetRenderDevice();

		Diligent::ShaderCreateInfo ShaderCI{};
		ShaderCI.ShaderCompiler = Diligent::SHADER_COMPILER_DEFAULT;
		ShaderCI.CompileFlags = Diligent::SHADER_COMPILE_FLAG_NONE;
		ShaderCI.EntryPoint = "main";
		std::string pathStr = path.string();
		ShaderCI.FilePath = pathStr.data();

		Diligent::IDataBlob* blob;
		Diligent::RefCntAutoPtr<Diligent::IShader> shader;
		diligentDevice->CreateShader(ShaderCI, &shader, &blob);
		if (blob)
		{
			std::string output = std::string((char*)blob->GetDataPtr(), blob->GetSize());
			MULE_LOG_VERBOSE("Shader {0} output: {1}", path, output);
		}
		return shader;
	}

	Shader::Shader(const SourceShaderDescription& desc)
		:
		mDevice(desc.Device),
		mContext(desc.Context)
	{
		
		mVertexShader = CreateShaderFromSource(desc.VertexPath);
		mFragmentShader = CreateShaderFromSource(desc.FragmentPath);


		Diligent::GraphicsPipelineStateCreateInfo PSOInfo{};
		PSOInfo.GraphicsPipeline.pRenderPass = desc.RenderPass->GetRenderPass();
		PSOInfo.GraphicsPipeline.PrimitiveTopology = Diligent::PRIMITIVE_TOPOLOGY::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		PSOInfo.GraphicsPipeline.SubpassIndex = desc.SubPassIndex;;
		PSOInfo.pVS = mVertexShader;
		PSOInfo.pPS = mFragmentShader;

	}
}