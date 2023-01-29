#include "mulepch.h"

#include "Mule/Renderer/Texture.h"
#include "Mule/Renderer/RenderAPI.h"

// Platforms

#include "Mule/Renderer/Vulkan/VulkanTexture1D.h"
#include "Mule/Renderer/Vulkan/VulkanTexture2D.h"
#include "Mule/Renderer/Vulkan/VulkanTexture3D.h"

namespace Mule
{
	Ref<Texture> Texture::CreateTexture1D(void* data, int bytes, TextureFormat format)
	{
		switch (RenderAPI::GetAPI())
		{
		case RenderAPI::API::VULKAN:
			return VulkanTexture1D::Create(data, bytes, format);
			break;
		}
		LOG_ERR(L"Invalid Render API");
		return nullptr;
	}
	Ref<Texture> Texture::CreateTexture2D(void* data, int width, int height, TextureFormat format)
	{
		switch (RenderAPI::GetAPI())
		{
		case RenderAPI::API::VULKAN:
			return VulkanTexture2D::Create(data, width, height, format);
			break;
		}
		LOG_ERR(L"Invalid Render API");
		return nullptr;
	}
	Ref<Texture> Texture::LoadTexture2D(const std::filesystem::path& filepath)
	{
		switch (RenderAPI::GetAPI())
		{
		case RenderAPI::API::VULKAN:
			return VulkanTexture2D::Load(filepath);
			break;
		}
		LOG_ERR(L"Invalid Render API");
		return nullptr;
	}
	Ref<Texture> Texture::LoadTexture2DAsync(const std::filesystem::path& filepath)
	{
		switch (RenderAPI::GetAPI())
		{
		case RenderAPI::API::VULKAN:
			return VulkanTexture2D::LoadAsync(filepath);
			break;
		}
		LOG_ERR(L"Invalid Render API");
		return nullptr;
	}
	Ref<Texture> Texture::CreateTexture3D(void* data, int width, int height, int depth, TextureFormat format)
	{
		switch (RenderAPI::GetAPI())
		{
		case RenderAPI::API::VULKAN:
			return VulkanTexture3D::Create(data, width, height, depth, format);
			break;
		}
		LOG_ERR(L"Invalid Render API");
		return nullptr;
	}
}