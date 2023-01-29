#pragma once

#include "IVulkanTexture.h"

namespace Mule
{
	class VulkanTexture1D : public IVulkanTexture
	{
	public:
		VulkanTexture1D(TextureType texType, TextureFormat format, int width, int height, int depth, int mipLevels, int arrayLayers);
		~VulkanTexture1D();

		// TODO: determine how we want to create these textures
		static Ref<VulkanTexture1D> Create(void* data, int width, TextureFormat format);


	private:
	};
}