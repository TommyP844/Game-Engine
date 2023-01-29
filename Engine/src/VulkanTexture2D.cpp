#include "mulepch.h"

#include "Mule/Renderer/Vulkan/VulkanTexture2D.h"
#include "Mule/Internal/ThreadPool.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace Mule
{
	VulkanTexture2D::VulkanTexture2D(TextureFormat format, int width, int height, int mipLevels, int arrayLayers)
		:
		IVulkanTexture(TextureType::TEXTURE_2D, format, width, height, 1, mipLevels, arrayLayers)
	{
	}
	VulkanTexture2D::~VulkanTexture2D()
	{
	}

	Ref<VulkanTexture2D> VulkanTexture2D::Create(void* data, int width, int height, TextureFormat format, int mipLevels, int arrayLayers)
	{
		Ref<VulkanTexture2D> texture = MakeRef<VulkanTexture2D>(format, width, height, mipLevels, arrayLayers);
		texture->CreateTexture(data);
		return texture;
	}
	
	Ref<VulkanTexture2D> VulkanTexture2D::Load(const std::filesystem::path& filepath, int mipLevels)
	{
		int width = 0, height = 0, comp = 0;
		void* data = stbi_load(filepath.u8string().c_str(), &width, &height, &comp, 0);
#ifdef DEBUG
		if (!data || width == 0 || height == 0 || comp == 0)
		{
			LOG_ERR(L"Failed to load texture \"{0}\"", filepath.wstring());
			if (data)
			{
				stbi_image_free(data);
			}
		}
#endif

		TextureFormat format;

		switch (comp)
		{
		case 1: format = TextureFormat::FORMAT_R8; break;
		case 2: format = TextureFormat::FORMAT_RG8; break;
		case 3: format = TextureFormat::FORMAT_RGB8; break;
		case 4: format = TextureFormat::FORMAT_RGBA8; break;
		}
		
		Ref<VulkanTexture2D> texture = MakeRef<VulkanTexture2D>(format, width, height, mipLevels, 1);
		texture->CreateTexture(data);
		return texture;

		stbi_image_free(data);
		return texture;
	}
	
	Ref<VulkanTexture2D> VulkanTexture2D::LoadAsync(const std::filesystem::path& filepath)
	{
		return nullptr;
	}
	
	void VulkanTexture2D::Save(const std::filesystem::path& filepath)
	{
	}

	void VulkanTexture2D::SaveAsync(const std::filesystem::path& filepath)
	{
	}
}