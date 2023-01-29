#pragma once

#include "IVulkanTexture.h"

namespace Mule
{
	class VulkanTexture3D : public IVulkanTexture
	{
	public:
		VulkanTexture3D(TextureType texType, TextureFormat format, int width, int height, int depth, int mipLevels, int arrayLayers);
		~VulkanTexture3D();

		static Ref<VulkanTexture3D> Create(void* data, int width, int height, int depth, TextureFormat format);


	private:
	};
}
