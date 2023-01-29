#pragma once

#include "Mule/Ref.h"
#include "Mule/Util/Log.h"
#include "Mule/Renderer/Texture.h"

#include <vulkan/vulkan.hpp>

#include <filesystem>

namespace Mule
{

	class IVulkanTexture : public Texture
	{
	public:
		IVulkanTexture(TextureType texType, TextureFormat format, int width, int height, int depth, int mipLevels, int arrayLayers);
		~IVulkanTexture();

		int GetWidth() const { return mWidth; }
		int GetHeight() const { return mHeight; }
		int GetDepth() const { return mDepth; }
		int GetMipLevels() const { return mMipLevels; }
		int GetArrayLayers() const { return mArrayLayers; }
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

		void SetData(void* data, int length);
		void* GetData();

		bool IsLoaded() const { return mIsLoaded; }
	protected:

		// Threadable function to create textures
		void CreateTexture(void* data);

	private:
		const TextureFormat mFormat;
		const TextureType mTextureType;
		bool mIsLoaded;
		const int mWidth, mHeight, mDepth, mMipLevels, mArrayLayers;
		
		vk::Image mTextureImage;
		vk::DeviceMemory mTextureImageMemory;

		uint64_t GetBytesPerPixel() const;
	};
}