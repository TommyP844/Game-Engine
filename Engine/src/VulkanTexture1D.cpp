#include "mulepch.h"
#include "Mule\Renderer\Vulkan\VulkanTexture1D.h"


namespace Mule
{

    VulkanTexture1D::VulkanTexture1D(TextureType texType, TextureFormat format, int width, int height, int depth, int mipLevels, int arrayLayers)
        :
        IVulkanTexture(texType, format, width, height, depth, mipLevels, arrayLayers)
    {
    }
    VulkanTexture1D::~VulkanTexture1D()
    {
    }
    Ref<VulkanTexture1D> VulkanTexture1D::Create(void* data, int width, TextureFormat format)
    {
        // TODO: refactor with mip levels and arraylayers
        Ref<VulkanTexture1D> texture = MakeRef<VulkanTexture1D>(TextureType::TEXTURE_1D, format, width, 0, 0, 1, 1);

        return texture;
    }
}
