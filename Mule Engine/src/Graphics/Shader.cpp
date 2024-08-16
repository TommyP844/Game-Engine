#include "Graphics/Shader.h"
#include "Log.h"
#include "DiligentCore/Graphics/GraphicsEngine/interface/Shader.h"
#include "DiligentCore/Graphics/GraphicsEngine/interface/Constants.h"
#include "Graphics/Vertex.h"
#include <fstream>

namespace Mule
{
	Ref<Shader> Shader::Create(const SourceShaderDescription& desc)
	{
		return Ref<Shader>(new Shader(desc));
	}

	VertexLayout Shader::GetDefaultVertexLayout()
	{
		/*
		 	struct Vertex
			{
			    glm::vec3 Position;
			    glm::vec2 TexCoord_0;
			    glm::vec3 Normal;
			    glm::vec3 Tangent;
			    // Bi-Tangent can be calculated in shader
			    glm::vec4 Color;
			};
		*/

		VertexLayout layout;

		layout.BufferIndex = 0;
		layout.PerInstance = false;
		// Position
		layout.Attribs.push_back(VertexAttrib(VertexAttribType::FLOAT, 3));
		// Tex Coord 0
		layout.Attribs.push_back(VertexAttrib(VertexAttribType::FLOAT, 2));
		// Normal
		layout.Attribs.push_back(VertexAttrib(VertexAttribType::FLOAT, 3));
		// Tangent
		layout.Attribs.push_back(VertexAttrib(VertexAttribType::FLOAT, 3));
		// Color
		layout.Attribs.push_back(VertexAttrib(VertexAttribType::FLOAT, 4));

		return layout;
	}

	void Shader::Bind()
	{
		mContext.GetContext()->SetPipelineState(mPipeline);
	}

	void Shader::BindResources()
	{
		
	}

	void Shader::SetUniformBuffer(ShaderStage stage, const std::string& bufferName, Ref<UniformBuffer> buffer)
	{
		auto var = mSRB->GetVariableByName((Diligent::SHADER_TYPE)stage, bufferName.c_str());
		if (var)
		{
			var->Set(buffer->GetBuffer());
		}
		else
		{
			MULE_LOG_ERROR("Shader buffer not found: {0} in shader {1}", bufferName, Name());
		}
	}

	void Shader::SetTexture(const std::string& name, Ref<Texture> texture)
	{
		auto ptr = mSRB->GetVariableByName(Diligent::SHADER_TYPE_ALL, name.c_str());
		ptr->Set(texture->GetShaderResourceView());
	}

	void Shader::SetArrayTexture(const std::string& name, Ref<Texture> texture, int index)
	{
		auto ptr = mSRB->GetVariableByName(Diligent::SHADER_TYPE_ALL, name.c_str());
		auto view = texture->GetShaderResourceView();
		Diligent::IDeviceObject* arr = view;
		ptr->SetArray(&arr, index, 1);
	}

	void Shader::SetShaderConstants(ShaderStage stage, Ref<ShaderConstant> buffer)
	{
		auto var = mPipeline->GetStaticVariableByName((Diligent::SHADER_TYPE)stage, buffer->GetName().c_str());
		if (var)
		{
			var->Set(buffer->GetBuffer());
		}
		else
		{
			MULE_LOG_ERROR("Failed to find buffer {0} in shader {1}", buffer->GetName(), Name());
		}
	}

	void Shader::Reload()
	{
	}

	Diligent::RefCntAutoPtr<Diligent::IShader> Shader::CreateShaderFromSource(const std::string& source, Diligent::SHADER_TYPE shaderType)
	{
		auto diligentDevice = mDevice->GetRenderDevice();

		Diligent::ShaderCreateInfo ShaderCI{};
		ShaderCI.EntryPoint = "main";
		ShaderCI.Source = source.data();
		ShaderCI.SourceLength = source.length();
		ShaderCI.Desc.ShaderType = shaderType;
		ShaderCI.SourceLanguage = Diligent::SHADER_SOURCE_LANGUAGE_HLSL;
		ShaderCI.LoadConstantBufferReflection = true;
		ShaderCI.ShaderCompiler = Diligent::SHADER_COMPILER_DEFAULT;
		ShaderCI.CompileFlags = Diligent::SHADER_COMPILE_FLAG_NONE;

		Diligent::IDataBlob* blob = nullptr;
		Diligent::RefCntAutoPtr<Diligent::IShader> shader;
		diligentDevice->CreateShader(ShaderCI, &shader, &blob);
		if (blob)
		{
			std::string output = std::string((char*)blob->GetDataPtr(), blob->GetSize());
			MULE_LOG_VERBOSE("Shader {0}\n\toutput: {1}", source, output);
		}

		return shader;
	}

	Shader::Shader(const SourceShaderDescription& desc)
		:
		mDevice(desc.Device),
		mContext(desc.Context),
		Asset(Asset::GenerateHandle(), desc.Path, AssetType::Shader)
	{
		auto diligentDevice = mDevice->GetRenderDevice();

		std::string vertexSource, fragmentSource, geometryource, tesselationControlSource, tesselationEvalSource;

		std::ifstream file(desc.Path);
		if (file.is_open())
		{
			enum ShaderType
			{
				Vertex,
				Fragment,
				Geometry,
				TesselationEvaluation,
				TesselationControl,
				None
			};
			ShaderType shaderType = None;
			std::string line;
			while (std::getline(file, line))
			{
				if (line == "#VERTEX")
				{
					shaderType = Vertex;
					continue;
				}
				else if (line == "#FRAGMENT")
				{
					shaderType = Fragment;
					continue;
				}
				line += '\n';

				switch (shaderType)
				{
				case Vertex: vertexSource += line; break;
				case Fragment: fragmentSource += line; break;
				case Geometry: geometryource += line; break;
				case TesselationEvaluation: tesselationEvalSource += line; break;
				case TesselationControl: tesselationControlSource += line; break;
				}
			}
			file.close();
		}
		else
		{
			return;
		}

		mVertexShader = CreateShaderFromSource(vertexSource, Diligent::SHADER_TYPE_VERTEX);
		mFragmentShader = CreateShaderFromSource(fragmentSource, Diligent::SHADER_TYPE::SHADER_TYPE_PIXEL);
		
		int count = mVertexShader->GetResourceCount();
		for (int i = 0; i < count; i++) {
			Diligent::ShaderResourceDesc d;
			mVertexShader->GetResourceDesc(i, d);
			std::cout << "Name: " <<d.Name << std::endl;
			std::cout << "Array Size: " <<d.ArraySize << std::endl;
		}

		Diligent::GraphicsPipelineStateCreateInfo PSOInfo{};
		PSOInfo.GraphicsPipeline.PrimitiveTopology = Diligent::PRIMITIVE_TOPOLOGY::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		PSOInfo.GraphicsPipeline.NumRenderTargets = 1;
		PSOInfo.GraphicsPipeline.RTVFormats[0] = Diligent::TEXTURE_FORMAT::TEX_FORMAT_RGBA8_UNORM;
		PSOInfo.GraphicsPipeline.DSVFormat = Diligent::TEXTURE_FORMAT::TEX_FORMAT_D32_FLOAT;
		PSOInfo.GraphicsPipeline.RasterizerDesc.CullMode = Diligent::CULL_MODE::CULL_MODE_BACK;
		PSOInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = true;
		std::vector<Diligent::LayoutElement> elements;

		for (int i = 0; i < desc.VertexLayouts.size(); i++)
		{
			auto& layout = desc.VertexLayouts[i];
			for (int j = 0; j < layout.Attribs.size(); j++)
			{
				auto& attrib = layout.Attribs[j];
				elements.push_back(Diligent::LayoutElement(
					j,
					layout.BufferIndex,
					attrib.Count, 
					(Diligent::VALUE_TYPE)attrib.Type,
					false,
					layout.PerInstance ? Diligent::INPUT_ELEMENT_FREQUENCY_PER_INSTANCE : Diligent::INPUT_ELEMENT_FREQUENCY_PER_VERTEX));
			}
		}
	
		PSOInfo.GraphicsPipeline.InputLayout.LayoutElements = elements.data();
		PSOInfo.GraphicsPipeline.InputLayout.NumElements = elements.size();
		PSOInfo.PSODesc.ResourceLayout.DefaultVariableType = Diligent::SHADER_RESOURCE_VARIABLE_TYPE_STATIC;
		PSOInfo.PSODesc.PipelineType = Diligent::PIPELINE_TYPE_GRAPHICS;
		
		PSOInfo.pVS = mVertexShader;
		PSOInfo.pPS = mFragmentShader;
		// TODO: add other stages

		diligentDevice->CreateGraphicsPipelineState(PSOInfo, &mPipeline);
	
		mPipeline->CreateShaderResourceBinding(&mSRB);
		int c = mSRB->GetVariableCount(Diligent::SHADER_TYPE_VERTEX);
	}
}