#include "mulepch.h"

#include "Mule/Renderer/Shader.h"
#include "Mule/Renderer/RenderAPI.h"
#include "Mule/Util/Log.h"

// Platform

#include "Mule/Renderer/Vulkan/VulkanShader.h"

namespace Mule
{

	

	Ref<Shader> Shader::Create(const ShaderDescription& shaderDesc)
	{
		switch (RenderAPI::GetAPI())
		{
		case RenderAPI::API::VULKAN:
			return VulkanShader::Create(shaderDesc);
			break;
		}
		LOG_ERR(L"Invalid API selected");
		return nullptr;
	}

	void Shader::Compile(const std::filesystem::path& shader, const std::filesystem::path& output)
	{
		std::string command = "C:\\VulkanSDK\\1.3.231.1\\Bin\\glslc.exe " + shader.u8string() + " -o " + output.u8string();
		system(command.c_str());
	}
}