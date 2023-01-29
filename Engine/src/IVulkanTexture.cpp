#include "mulepch.h"

#include "Mule/Renderer/Vulkan/IVulkanTexture.h"
#include "Mule/Renderer/RenderAPI.h"

namespace Mule
{
	IVulkanTexture::IVulkanTexture(TextureType texType, TextureFormat format,
		int width, int height, int depth, int mipLevels, int arrayLayers)
		:
		mWidth(width),
		mHeight(height),
		mDepth(depth),
		mMipLevels(mipLevels),
		mArrayLayers(arrayLayers),
		mIsLoaded(false),
		mFormat(format),
		mTextureType(texType),
		mTextureImage(nullptr),
		mTextureImageMemory(nullptr)
	{
	}
	
	IVulkanTexture::~IVulkanTexture()
	{
		vk::Device device = RenderAPI::GetDevice();
		device.destroyImage(mTextureImage);
		device.freeMemory(mTextureImageMemory);
	}

	void IVulkanTexture::SetData(void* data, int length)
	{
	}

	void* IVulkanTexture::GetData()
	{
		return nullptr;
	}

	void IVulkanTexture::CreateTexture(void* pixels)
	{
		vk::Buffer stagingBuffer{};
		vk::DeviceMemory stagingBufferMemory{};
		
		VkDeviceSize imageSize = static_cast<uint64_t>(mWidth) * static_cast<uint64_t>(mHeight) * static_cast<uint64_t>(mDepth) * GetBytesPerPixel();
		RenderAPI::CreateBuffer(
			imageSize,
			vk::BufferUsageFlagBits::eTransferSrc,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
			stagingBuffer, 
			stagingBufferMemory);
		vk::Device device = RenderAPI::GetDevice();

		void* data = new unsigned char[imageSize];
		device.mapMemory(stagingBufferMemory, vk::DeviceSize(0), imageSize, vk::MemoryMapFlags(0));
		memcpy(data, pixels, static_cast<size_t>(imageSize));
		device.unmapMemory(stagingBufferMemory);
				

		vk::ImageType imageType{};
		switch (mTextureType)
		{
		case TextureType::TEXTURE_1D: imageType = vk::ImageType::e1D; break;
		case TextureType::TEXTURE_2D: imageType = vk::ImageType::e2D; break;
		case TextureType::TEXTURE_3D: imageType = vk::ImageType::e3D; break;
		default:
			LOG_ERR(L"Invalid texture type");
			break;
		}

		vk::ImageCreateInfo imageInfo{};
		imageInfo.sType = vk::StructureType::eImageCreateInfo;
		imageInfo.imageType = imageType;
		imageInfo.extent.width = static_cast<uint32_t>(mWidth);
		imageInfo.extent.height = static_cast<uint32_t>(mHeight);
		imageInfo.extent.depth = static_cast<uint32_t>(mDepth);
		imageInfo.mipLevels = static_cast<uint32_t>(mMipLevels);
		imageInfo.arrayLayers = static_cast<uint32_t>(mArrayLayers);

		vk::Format format{};
		switch (mFormat)
		{
		case TextureFormat::FORMAT_R8: format = vk::Format::eR8Srgb; break;
		case TextureFormat::FORMAT_RG8: format = vk::Format::eR8G8Srgb; break;
		case TextureFormat::FORMAT_RGB8: format = vk::Format::eR8G8B8Srgb; break;
		case TextureFormat::FORMAT_RGBA8: format = vk::Format::eR8G8B8A8Srgb; break;

		case TextureFormat::FORMAT_R32I: format = vk::Format::eR32Sint; break;
		case TextureFormat::FORMAT_RG32I: format = vk::Format::eR32G32Sint; break;
		case TextureFormat::FORMAT_RGB32I: format = vk::Format::eR32G32B32Sint; break;
		case TextureFormat::FORMAT_RGBA32I: format = vk::Format::eR32G32B32A32Sint; break;

		case TextureFormat::FORMAT_R16F: format = vk::Format::eR16Sfloat; break;
		case TextureFormat::FORMAT_RG16F: format = vk::Format::eR16G16Sfloat; break;
		case TextureFormat::FORMAT_RGB16F: format = vk::Format::eR16G16B16Sfloat; break;
		case TextureFormat::FORMAT_RGBA16F: format = vk::Format::eR16G16B16A16Sfloat; break;

		case TextureFormat::FORMAT_R32F: format = vk::Format::eR32Sfloat; break;
		case TextureFormat::FORMAT_RG32F: format = vk::Format::eR32G32Sfloat; break;
		case TextureFormat::FORMAT_RGB32F: format = vk::Format::eR32G32B32Sfloat; break;
		case TextureFormat::FORMAT_RGBA32F: format = vk::Format::eR32G32B32A32Sfloat; break;

		default:
			LOG_ERR(L"Invalid texture format");
			break;
		}

		imageInfo.format = format;
		imageInfo.tiling = vk::ImageTiling::eOptimal;
		imageInfo.initialLayout = vk::ImageLayout::eUndefined;
		imageInfo.usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
		imageInfo.sharingMode = vk::SharingMode::eExclusive;
		imageInfo.samples = vk::SampleCountFlagBits::e1;
		imageInfo.flags = vk::ImageCreateFlags(0); // Optional

		mTextureImage = device.createImage(imageInfo);

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(device, mTextureImage, &memRequirements);


		vk::MemoryAllocateInfo allocInfo{};
		allocInfo.sType = vk::StructureType::eMemoryAllocateInfo;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = RenderAPI::FindMemoryType(memRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal);

		mTextureImageMemory =  device.allocateMemory(allocInfo);

		device.bindImageMemory(mTextureImage, mTextureImageMemory, vk::DeviceSize(0));

		device.destroyBuffer(stagingBuffer);
		device.freeMemory(stagingBufferMemory);
	}

	uint64_t IVulkanTexture::GetBytesPerPixel() const
	{
		switch (mFormat)
		{
		case TextureFormat::FORMAT_R8: return 1ull; break;
		case TextureFormat::FORMAT_RG8: return 2ull; break;
		case TextureFormat::FORMAT_RGB8: return 3ull; break;
		case TextureFormat::FORMAT_RGBA8: return 4ull; break;

		case TextureFormat::FORMAT_R16F: return 1 * 2ull; break;
		case TextureFormat::FORMAT_RG16F: return 2 * 2ull; break;
		case TextureFormat::FORMAT_RGB16F: return 3 * 2ull; break;
		case TextureFormat::FORMAT_RGBA16F: return 4 * 2ull; break;

		case TextureFormat::FORMAT_R32F: return 1 * 4ull; break;
		case TextureFormat::FORMAT_RG32F: return 2 * 4ull; break;
		case TextureFormat::FORMAT_RGB32F: return 3 * 4ull; break;
		case TextureFormat::FORMAT_RGBA32F: return 4 * 4ull; break;

		case TextureFormat::FORMAT_R32I: return 1 * 4ull; break;
		case TextureFormat::FORMAT_RG32I: return 2 * 4ull; break;
		case TextureFormat::FORMAT_RGB32I: return 3 * 4ull; break;
		case TextureFormat::FORMAT_RGBA32I: return 4 * 4ull; break;
		}
		LOG_ERR(L"Invalid texture format");
		return 0;
	}
}
