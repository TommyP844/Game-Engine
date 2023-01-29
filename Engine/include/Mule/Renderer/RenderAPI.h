#pragma once

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>

#include <GLFW/glfw3.h>

#include <vector>

#include "Shader.h"
#include "Mule/Ref.h"

namespace Mule
{
	struct VulkanData
	{
		vk::Instance Instance;
		vk::PhysicalDevice PhysicalDevice;
		vk::Device LogicalDevice;
		vk::SurfaceKHR Surface;
		vk::Queue GraphicsQueue;
		vk::Queue PresentQueue;
		vk::SwapchainKHR SwapChain;
		std::vector<vk::Image> SwapChainImages;
		vk::Format SwapChainFormat;
		vk::Extent2D SwapChainExtent;
		std::vector<vk::ImageView> SwapChainImageViews;
		vk::RenderPass RenderPass;
		vk::SampleCountFlagBits MSAASamples = vk::SampleCountFlagBits::e4;
		vk::DescriptorSetLayout DesciptorLayout;
		std::vector<vk::Framebuffer> SwapChainFramebuffers;
		vk::Image ColorImage;
		vk::ImageView ColorImageView;
		vk::DeviceMemory ColorImageMemory;

		vk::Image DepthImage;
		vk::ImageView DepthImageView;
		vk::DeviceMemory DepthImageMemory;

		std::vector<vk::CommandBuffer> CommandBuffers;

		vk::CommandPool CommandPool;

		vk::DescriptorPool DescriptorPool;
		vk::DescriptorPool ImguiPool;

		uint32_t CurrentFrame = 0;

		std::vector<vk::Semaphore> ImageAvailableSemaphores;
		std::vector<vk::Semaphore> RenderFinishedSemaphores;
		std::vector<vk::Fence> InFlightFences;

		uint32_t ImageIndex;

		vk::CommandBuffer ActiveCommandBuffer;
	};

	class RenderAPI
	{
	public:

		enum class API
		{
			NONE,
			VULKAN
		};

		static void Init(API api, GLFWwindow* window);
		static void Shutdown();

		static API GetAPI() { return sRenderAPI; }

		static int GetSwapChainBufferCount() { return BUFFER_COUNT; }

		static void BeginFrame();
		static void SubmitCommand(const std::function<void()>& command);
		static void EndFrame();
		static void ExecuteCommands();

		static void InitImGui();
		static void ShutdownImGui();
		static void BeginImGuiFrame();
		static void EndImGuiFrame();

		static int GetCurrentSwapChainFrame();

		static void SwapBuffers();

		// For vulkan objects
		static vk::RenderPass GetRenderPass();
		static const vk::Device& GetDevice();
		static const vk::PhysicalDevice& GetPhysicalDevice();
		static void CopyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size);
		static void CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& bufferMemory);
		static uint32_t FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);
		static vk::CommandBuffer GetActiveCommandBuffer();
		static vk::Extent2D GetSwapChainExtent();
		static vk::SampleCountFlagBits GetSamples();
		static vk::DescriptorPool GetDescriptorPool();
		

	private:
		static const int BUFFER_COUNT = 2;
		static API sRenderAPI;
		static GLFWwindow* sWindow;
		static std::vector<std::function<void()>> sRenderCommands;

		// Vulkan

		struct VulkanQueueFamilyIndices
		{
			uint32_t GraphicsIndex;
			uint32_t PresentIndex;

			bool HasPresentationQueue;
		};

		struct VulkanSwapChainSupportDetails {
			vk::SurfaceCapabilitiesKHR Capabilities;
			std::vector<vk::SurfaceFormatKHR> Formats;
			std::vector<vk::PresentModeKHR> PresentModes;
		};

		static VulkanData sVulkanData;

		static void InitVulkan();
		static void ShutdownVulkan();
		static void InitImguiForVulkan();
		static void CreateVulkanInstance();
		static std::vector<const char*> GetRequiredVulkanExtensions();
		static std::vector<const char*> GetRequiredDeviceExtensions();
		static std::vector<const char*> GetValidationLayers();
		static void PickPhysicalDevice();
		static void CreateLogicalDevice();
		static VulkanQueueFamilyIndices FindQueueFamilies();
		static void CreateSwapChain();
		static VulkanSwapChainSupportDetails QuerySwapChainSupportDetails();
		static vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
		static vk::PresentModeKHR ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);
		static vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities);
		static void CreateImage(uint32_t width, uint32_t height, uint32_t mipLevels, vk::SampleCountFlagBits numSamples, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Image& image, vk::DeviceMemory& imageMemory);
		static vk::ImageView CreateImageView(vk::Image image, vk::Format format, vk::ImageAspectFlags aspectFlags, uint32_t mipLevels);
		static void CreateRenderPass();
		static vk::Format FindDepthFormat();
		static vk::Format FindSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features);
		static void CreateDescriptorSetLayout();
		static void CreateFrameBuffers();
		static void CreateColorResource();
		static void CreateDepthResource();
		static void CreateCommandBuffer();
		static void CreateCommandPool();
		static void CreateSyncObjects();
		static VkCommandBuffer BeginSingleTimeCommands();
		static void EndSingleTimeCommands(vk::CommandBuffer commandBuffer);
		static void RecreateSwapChain();
		static void DestroySwapChain();
		static void CreateDescriptorPool();
	};
	
}