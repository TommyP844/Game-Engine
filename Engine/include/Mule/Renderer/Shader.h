#pragma once

#include "Mule/Ref.h"
#include "Mule/Renderer/Texture.h"

#include "bgfx/bgfx.h"

#include <glm/glm.hpp>

#include <string>
#include <filesystem>
#include <map>

namespace fs = std::filesystem;

namespace Mule
{

	enum UniformType
	{
		Mat3 = bgfx::UniformType::Mat3,
		Mat4 = bgfx::UniformType::Mat4,
		Vec4 = bgfx::UniformType::Vec4,
		Sampler = bgfx::UniformType::Sampler
	};

	class Shader
	{
	public:
		Shader();
		~Shader();

		static Ref<Shader> Create(const fs::path& filepath);

		const std::string& GetName() const { return mName; }
		static Ref<Shader> GetDefault();
		void Draw();
		void Reload();
		void CreateUniform(const std::string& name, UniformType type, int arrayCount = 0);
		void SetUniform(const std::string& name, void* data);
		void SetMVP(const glm::mat4& model, const glm::mat4& view, const glm::mat4& proj);
		void SetTexture(const std::string& name, Ref<Texture> texture, int slot = 0);

	private:
		static void LoadShaderCode(char** code, int& vsSize, const std::string& file);

		std::string mName;
		fs::path mPath;
		bgfx::ProgramHandle mHandle;
		std::map<std::string, bgfx::UniformHandle> mUniforms;
	};
}