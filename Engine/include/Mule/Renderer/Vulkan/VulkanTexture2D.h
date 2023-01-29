#pragma once

#include "IVulkanTexture.h"

#include <vulkan/vulkan.hpp>

namespace Mule
{
	class VulkanTexture2D : public IVulkanTexture
	{
	public:
		VulkanTexture2D(TextureFormat format, int width, int height, int mipLevels, int arrayLayers);
		~VulkanTexture2D();

		static Ref<VulkanTexture2D> Create(void* data, int width, int height, TextureFormat format, int mipLevels = 1, int arrayLayers = 1);

		static Ref<VulkanTexture2D> Load(const std::filesystem::path& filepath, int mipLevels = 1);
		static Ref<VulkanTexture2D> LoadAsync(const std::filesystem::path& filepath);
		static void Save(const std::filesystem::path& filepath);
		static void SaveAsync(const std::filesystem::path& filepath);

	private:
	};
}