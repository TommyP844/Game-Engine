#pragma once

#include "GraphicsDevice.h"
#include "Util/Buffer.h"
#include "RenderPass.h"

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
		fs::path VertexPath;
		fs::path FragmentPath;
	};

	struct BinaryShaderDescription
	{
		RenderContext Context;
		WeakRef<GraphicsDevice> Device;

		std::string Name;
		Buffer<uint8_t> VertexCode;
		Buffer<uint8_t> FragmentCode;
	};

	class Shader
	{
	public:
		static Ref<Shader> Create(const SourceShaderDescription& desc);
		static Ref<Shader> Create(const BinaryShaderDescription& desc);


		void Reload();

	private:
		Shader(const SourceShaderDescription& desc);
		Shader(const BinaryShaderDescription& desc);

		WeakRef<GraphicsDevice> mDevice;
		RenderContext mContext;

		Diligent::RefCntAutoPtr<Diligent::IShader> CreateShaderFromSource(const fs::path& path);
		Diligent::RefCntAutoPtr<Diligent::IShader> CreateShaderFromBinary(Buffer<uint8_t> code);

		Diligent::RefCntAutoPtr<Diligent::IShader> mVertexShader;
		Diligent::RefCntAutoPtr<Diligent::IShader> mFragmentShader;
	};
}