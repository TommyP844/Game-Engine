
#include "mulepch.h"

#include "Mule/Renderer/Shader.h"
#include "Mule/Util/Log.h"

#include <fstream>

namespace Mule
{
	Shader::Shader()
		:
		mHandle()
	{
	}

	Shader::~Shader()
	{
		bgfx::destroy(mHandle);
		for (auto& uniform : mUniforms)
		{
			bgfx::destroy(uniform.second);
		}
	}

	Ref<Shader> Shader::GetDefault()
	{
		static Ref<Shader> shader = nullptr;
		if (shader == nullptr)
		{
			LOG_MSG(L"Creating default shader");
			// create all our uniforms
			shader = Create("../Resource/Shaders/pbr.shader");
			shader->CreateUniform("u_mvp", Mat4);
			shader->CreateUniform("AlbedoMap", Sampler);
			shader->CreateUniform("NormalMap", Sampler);
			shader->CreateUniform("MetalnessMap", Sampler);
			shader->CreateUniform("RoughnessMap", Sampler);
			shader->CreateUniform("AOMap", Sampler);
		}
		return shader;
	}

	void Shader::Draw()
	{
		bgfx::submit(0, mHandle, 0, 
			BGFX_DISCARD_INDEX_BUFFER
			| BGFX_DISCARD_VERTEX_STREAMS);
	}

	void Shader::Reload()
	{
		bgfx::destroy(mHandle);
		for (auto& uniform : mUniforms)
		{
			bgfx::destroy(uniform.second);
		}
	}

	void Shader::CreateUniform(const std::string& name, UniformType type, int arrayCount)
	{
		bgfx::UniformHandle handle = bgfx::createUniform(name.c_str(), (bgfx::UniformType::Enum)type, arrayCount == 0 ? 1 : arrayCount);
		if (!bgfx::isValid(handle))
		{
			LOG_ERR(L"Failed to create uniform: {0}", name);
		}
		mUniforms[name] = handle;
	}

	void Shader::SetUniform(const std::string& name, void* data)
	{
		bgfx::setUniform(mUniforms[name], data);
	}

	void Shader::SetMVP(const glm::mat4& model, const glm::mat4& view, const glm::mat4& proj)
	{
		bgfx::setViewTransform(0, &view[0][0], &proj[0][0]);
	}

	void Shader::SetTexture(const std::string& name, Ref<Texture> texture, int slot)
	{
		bgfx::setTexture(slot, mUniforms[name.c_str()], texture->GetHandle());
	}

	void Shader::LoadShaderCode(char** code, int& vsSize, const std::string& file)
	{
		std::ifstream shader(file);
		if (!shader.is_open())
		{
			LOG_ERR(L"Failed to open shader file: {0}", file);
			return;
		}

		shader.seekg(0, std::ios::end);
		vsSize = shader.tellg();
		shader.seekg(0, std::ios::beg);
		*code = new char[vsSize];
		shader.read(*code, vsSize);
	}

	Ref<Shader> Mule::Shader::Create(const fs::path& filepath)
	{
		std::ifstream file(filepath);
		if (!file.is_open())
		{
			LOG_ERR(L"Failed to open shader: {0}", filepath.u8string());
			return nullptr;
		}
		std::string vsName;
		std::string fsName;

		std::string line;
		while (std::getline(file, line))
		{
			if (line.substr(0, 2) == "vs")
			{
				line = line.substr(line.find_first_of("vs:") + 3);
				int offset = 0;
				while (line[offset] == ' ') offset++;
				vsName = filepath.parent_path().u8string() + "\\" + line.substr(offset);

			}
			if (line.substr(0, 2) == "fs")
			{
				line = line.substr(line.find_first_of("fs:") + 3);
				int offset = 0;
				while (line[offset] == ' ') offset++;
				fsName = filepath.parent_path().u8string() + "\\" + line.substr(offset);
			}

		}
		int vsSize = 0;
		char* vs = NULL;
		LoadShaderCode(&vs, vsSize, vsName);

		int fsSize = 0;
		char* fs = NULL;
		LoadShaderCode(&fs, fsSize, fsName);


		bgfx::ReleaseFn func = [](void* ptr, void* userData) {
			delete[] ptr;
		};

		bgfx::ShaderHandle vertex = bgfx::createShader(bgfx::makeRef(vs, vsSize, func));
		bgfx::ShaderHandle frag = bgfx::createShader(bgfx::makeRef(fs, fsSize, func));

		Ref<Shader> shader = MakeRef<Shader>();

		shader->mHandle = bgfx::createProgram(vertex, frag, true);
		if (!bgfx::isValid(shader->mHandle))
		{
			LOG_ERR(L"Shader handle invalid: {0}", filepath.u8string());
		}

		return shader;
	}

}