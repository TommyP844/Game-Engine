#pragma once

#include "Mule/Ref.h"
#include "Mule/Util/Log.h"

#include <filesystem>

namespace Mule
{
	enum class TextureType : int
	{
		TEXTURE_1D,
		TEXTURE_2D,
		TEXTURE_3D,
		TEXTURE_HDR,
	};

	enum class TextureFormat : int
	{
		FORMAT_R8,
		FORMAT_RG8,
		FORMAT_RGB8,
		FORMAT_RGBA8,

		FORMAT_R32F,
		FORMAT_RG32F,
		FORMAT_RGB32F,
		FORMAT_RGBA32F,

		FORMAT_R16F,
		FORMAT_RG16F,
		FORMAT_RGB16F,
		FORMAT_RGBA16F,

		FORMAT_R32I,
		FORMAT_RG32I,
		FORMAT_RGB32I,
		FORMAT_RGBA32I

	};

	class ITexture
	{
	public:
		ITexture(TextureType texType) : mIsLoaded(false), mTextureType(texType) {}
		virtual ~ITexture(){}

		virtual int GetWidth() const = 0;
		virtual int GetHeight() const = 0;
		virtual int GetDepth() const = 0;
		int GetComponents() const
		{
			switch (mFormat)
			{
			case TextureFormat::FORMAT_R8:
			case TextureFormat::FORMAT_R16F:
			case TextureFormat::FORMAT_R32F:
			case TextureFormat::FORMAT_R32I:
				return 1;
				break;
			case TextureFormat::FORMAT_RG8:
			case TextureFormat::FORMAT_RG16F:
			case TextureFormat::FORMAT_RG32F:
			case TextureFormat::FORMAT_RG32I:
				return 2;
				break;
			case TextureFormat::FORMAT_RGB8:
			case TextureFormat::FORMAT_RGB16F:
			case TextureFormat::FORMAT_RGB32F:
			case TextureFormat::FORMAT_RGB32I:
				return 3;
				break;
			case TextureFormat::FORMAT_RGBA8:
			case TextureFormat::FORMAT_RGBA16F:
			case TextureFormat::FORMAT_RGBA32F:
			case TextureFormat::FORMAT_RGBA32I:
				return 4;
				break;
			default:
				LOG_ERR(L"Invalid texture format {0}", (int)mFormat);
				break;
			}
		}

		virtual void SetData(void* data, int length) = 0;
		virtual void* GetData() = 0;
		
		virtual Ref<ITexture> Load(const std::filesystem::path& filepath) = 0;
		virtual Ref<ITexture> LoadAsync(const std::filesystem::path& filepath) = 0;
		virtual void Save(const std::filesystem::path& filepath) = 0;
		virtual void SaveAsync(const std::filesystem::path& filepath) = 0;

		bool IsLoaded() const { return mIsLoaded; }
	protected:
		TextureFormat mFormat;
		const TextureType mTextureType;
		bool mIsLoaded;
	};
}