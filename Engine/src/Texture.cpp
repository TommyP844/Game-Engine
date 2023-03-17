#include "mulepch.h"
#include "Mule/Renderer/Texture.h"

#include "Mule/Util/Log.h"

#include "Mule/bgfxImGui/imgui.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace Mule
{
	Texture::Texture()
	{
	}

	Texture::~Texture()
	{
		if (mHandle.idx != bgfx::kInvalidHandle)
		{
			bgfx::destroy(mHandle);
		}
	}

	Ref<Texture> Texture::Create(const fs::path& filepath)
	{
		TextureInfo info;
		int comp = 0;
		void* data = stbi_load(filepath.u8string().c_str(), &info.Width, &info.Height, &comp, 4);
		if (info.Width == 0 || info.Height == 0 || comp == 0 || data == nullptr)
		{
			LOG_ERR(L"Failed to load texture {0}", filepath.u8string());
			return nullptr;
		}
		info.Format = TextureFormat::RGBA8;
		info.pixelSize = 4;
		info.Layers = 1;
		info.Depth = 1;
		info.Data = data;
		std::string name = filepath.filename().u8string();
		info.Name = name.substr(0, name.find_last_of("."));

		return Create(info);
	}

	Ref<Texture> Texture::Create(const TextureInfo& info)
	{
		if (info.Name.empty())
		{
			LOG_ERR(L"Textures must have a name");
			return nullptr;
		}

		bgfx::TextureHandle handle = bgfx::TextureHandle();
		bgfx::TextureFormat::Enum format = *(bgfx::TextureFormat::Enum*)&info.Format;

		if (info.Width > 1 && info.Height == 0 && info.Depth == 0)
		{
			
		}
		else if (info.Width > 1 && info.Height > 1 && info.Depth == 1)
		{
			handle = bgfx::createTexture2D(
				info.Width,
				info.Height,
				false,
				info.Layers,
				format,
				0,
				info.Data ? bgfx::makeRef(info.Data, info.Width * info.Height * info.Layers * info.pixelSize) : 0
				);
		}
		else if (info.Width > 1 && info.Height > 1 && info.Depth > 1)
		{
			handle = bgfx::createTexture3D(
				info.Width,
				info.Height,
				info.Depth,
				false,
				format,
				0,
				info.Data ? bgfx::makeRef(info.Data, info.Width * info.Height * info.Layers * info.pixelSize) : 0
			);
		}

		if (handle.idx == bgfx::kInvalidHandle)
		{
			LOG_ERR(L"Failed create texture");
			return nullptr;
		}

		Ref<Texture> texture = MakeRef<Texture>();

		texture->mHandle = handle;
		texture->mWidth = info.Width;
		texture->mHeight = info.Height;
		texture->mDepth = info.Depth;
		texture->mLayers = info.Layers;
		texture->mFormat = info.Format;
		texture->mPixelSize = info.pixelSize;
		texture->mName = info.Name;

		return texture;
	}

	ImTextureID Texture::GetImguiID()
	{
		return ImGui::toId(mHandle, 0, 0);
	}
}