#include "mulepch.h"

#include "Mule\Renderer\Vulkan\VulkanTexture3D.h"

namespace Mule
{
	VulkanTexture3D::VulkanTexture3D(TextureType texType, TextureFormat format, int width, int height, int depth, int mipLevels, int arrayLayers)
		:
		IVulkanTexture(texType, format, width, height, depth, mipLevels, arrayLayers)
	{
	}
	VulkanTexture3D::~VulkanTexture3D()
	{
	}

	Ref<VulkanTexture3D> VulkanTexture3D::Create(void* data, int width, int height, int depth, TextureFormat format)
	{
		// TODO: refactor with mip levels and arraylayers
		Ref<VulkanTexture3D> texture = MakeRef<VulkanTexture3D>(TextureType::TEXTURE_3D, format, width, height, depth, 1, 1);

		return texture;
	}
}