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
		VkDevice device = RenderAPI::GetDevice();
		vkDestroyImage(device, mTextureImage, nullptr);
		vkFreeMemory(device, mTextureImageMemory, nullptr);
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
		VkBuffer stagingBuffer{};
		VkDeviceMemory stagingBufferMemory{};
		
		VkDeviceSize imageSize = static_cast<uint64_t>(mWidth) * static_cast<uint64_t>(mHeight) * static_cast<uint64_t>(mDepth) * GetBytesPerPixel();
		RenderAPI::CreateBuffer(
			imageSize,
			VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer, 
			stagingBufferMemory);
		VkDevice device = RenderAPI::GetDevice();

		void* data = NULL;  
		vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
		memcpy(data, pixels, static_cast<size_t>(imageSize));
		vkUnmapMemory(device, stagingBufferMemory);
				

		VkImageType imageType{};
		switch (mTextureType)
		{
		case TextureType::TEXTURE_1D: imageType = VkImageType::VK_IMAGE_TYPE_1D; break;
		case TextureType::TEXTURE_2D: imageType = VkImageType::VK_IMAGE_TYPE_2D; break;
		case TextureType::TEXTURE_3D: imageType = VkImageType::VK_IMAGE_TYPE_3D; break;
		default:
			LOG_ERR(L"Invalid texture type");
			break;
		}

		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = imageType;
		imageInfo.extent.width = static_cast<uint32_t>(mWidth);
		imageInfo.extent.height = static_cast<uint32_t>(mHeight);
		imageInfo.extent.depth = static_cast<uint32_t>(mDepth);
		imageInfo.mipLevels = static_cast<uint32_t>(mMipLevels);
		imageInfo.arrayLayers = static_cast<uint32_t>(mArrayLayers);

		VkFormat format{};
		switch (mFormat)
		{
		case TextureFormat::FORMAT_R8: format = VkFormat::VK_FORMAT_R8_SRGB; break;
		case TextureFormat::FORMAT_RG8: format = VkFormat::VK_FORMAT_R8G8_SRGB; break;
		case TextureFormat::FORMAT_RGB8: format = VkFormat::VK_FORMAT_R8G8B8_SRGB; break;
		case TextureFormat::FORMAT_RGBA8: format = VkFormat::VK_FORMAT_R8G8B8A8_SRGB; break;

		case TextureFormat::FORMAT_R32I: format = VkFormat::VK_FORMAT_R32_SINT; break;
		case TextureFormat::FORMAT_RG32I: format = VkFormat::VK_FORMAT_R32G32_SINT; break;
		case TextureFormat::FORMAT_RGB32I: format = VkFormat::VK_FORMAT_R32G32B32_SINT; break;
		case TextureFormat::FORMAT_RGBA32I: format = VkFormat::VK_FORMAT_R32G32B32_SINT; break;

		case TextureFormat::FORMAT_R16F: format = VkFormat::VK_FORMAT_R16_SFLOAT; break;
		case TextureFormat::FORMAT_RG16F: format = VkFormat::VK_FORMAT_R16G16_SFLOAT; break;
		case TextureFormat::FORMAT_RGB16F: format = VkFormat::VK_FORMAT_R16G16B16_SFLOAT; break;
		case TextureFormat::FORMAT_RGBA16F: format = VkFormat::VK_FORMAT_R16G16B16A16_SFLOAT; break;

		case TextureFormat::FORMAT_R32F: format = VkFormat::VK_FORMAT_R32_SFLOAT; break;
		case TextureFormat::FORMAT_RG32F: format = VkFormat::VK_FORMAT_R32G32_SFLOAT; break;
		case TextureFormat::FORMAT_RGB32F: format = VkFormat::VK_FORMAT_R32G32B32_SFLOAT; break;
		case TextureFormat::FORMAT_RGBA32F: format = VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT; break;

		default:
			LOG_ERR(L"Invalid texture format");
			break;
		}

		imageInfo.format = format;
		imageInfo.tiling = VkImageTiling::VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT;
		imageInfo.sharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.samples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
		imageInfo.flags = VkImageCreateFlags(0); // Optional

		vkCreateImage(device, &imageInfo, nullptr, &mTextureImage);

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(device, mTextureImage, &memRequirements);


		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = RenderAPI::FindMemoryType(memRequirements.memoryTypeBits, VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		vkAllocateMemory(device, &allocInfo, nullptr, &mTextureImageMemory);

		vkBindImageMemory(device, mTextureImage, mTextureImageMemory, 0);

		vkDestroyBuffer(device, stagingBuffer, nullptr);
		vkFreeMemory(device, stagingBufferMemory, nullptr);
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
