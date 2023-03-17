#pragma once

#include "Mule/Ref.h"

#include "bgfx/bgfx.h"

#include <filesystem>

namespace fs = std::filesystem;

namespace Mule
{
	enum TextureFormat
	{
		R8 = bgfx::TextureFormat::R8,
		RG8 = bgfx::TextureFormat::RG8,
		RGB8 = bgfx::TextureFormat::RGB8,
		RGBA8 = bgfx::TextureFormat::RGBA8,

		R16F = bgfx::TextureFormat::R16F,
		RG16F = bgfx::TextureFormat::RG16F,
		// RGB16F = bgfx::TextureFormat::RGB16F,
		RGBA16F = bgfx::TextureFormat::RGBA16F,

		R32F = bgfx::TextureFormat::R32F,
		RG32F = bgfx::TextureFormat::RG32F,
		// RGB32F = bgfx::TextureFormat::RGB32F,
		RGBA32F = bgfx::TextureFormat::RGBA32F,

		D32F = bgfx::TextureFormat::D32F


	};

	struct TextureInfo
	{
		int Width = 0;
		int Height = 1;
		int Depth = 1;
		int Layers = 1;
		int pixelSize;
		TextureFormat Format;
		void* Data = nullptr;
		std::string Name;
	};

	class Texture
	{
	public:
		Texture();
		~Texture();

		static Ref<Texture> Create(const fs::path& filepath);
		static Ref<Texture> Create(const TextureInfo& info);

		void* GetImguiID();

		bgfx::TextureHandle GetHandle() const { return mHandle; }

		int GetWidth() const { return mWidth; }
		int GetHeight() const { return mHeight; }
		int GetDepth() const { return mDepth; }

		int GetLayers() const { return mLayers; }

		int GetPizelSize() const { return mPixelSize; }

		TextureFormat GetFormat() const { return mFormat; }

		const std::string& GetName() const { return mName; }

	private:
		bgfx::TextureHandle mHandle;
		int mWidth, mHeight, mDepth, mLayers;
		TextureFormat mFormat;
		int mPixelSize;
		std::string mName;
	};
}