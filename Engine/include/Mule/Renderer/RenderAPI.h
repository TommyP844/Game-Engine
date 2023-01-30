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
		VkInstance Instance;
		VkPhysicalDevice PhysicalDevice;
		VkDevice LogicalDevice;
		VkSurfaceKHR Surface;
		VkQueue GraphicsQueue;
		VkQueue PresentQueue;
		VkSwapchainKHR SwapChain;
		std::vector<VkImage> SwapChainImages;
		VkFormat SwapChainFormat;
		VkExtent2D SwapChainExtent;
		std::vector<VkImageView> SwapChainImageViews;
		VkRenderPass RenderPass;
		VkSampleCountFlagBits MSAASamples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_4_BIT;
		VkDescriptorSetLayout DesciptorLayout;
		std::vector<VkFramebuffer> SwapChainFramebuffers;
		VkImage ColorImage;
		VkImageView ColorImageView;
		VkDeviceMemory ColorImageMemory;

		VkImage DepthImage;
		VkImageView DepthImageView;
		VkDeviceMemory DepthImageMemory;

		std::vector<VkCommandBuffer> CommandBuffers;

		VkCommandPool CommandPool;

		VkDescriptorPool DescriptorPool;
		VkDescriptorPool ImguiPool;

		uint32_t CurrentFrame = 0;

		std::vector<VkSemaphore> ImageAvailableSemaphores;
		std::vector<VkSemaphore> RenderFinishedSemaphores;
		std::vector<VkFence> InFlightFences;

		uint32_t ImageIndex;

		VkCommandBuffer ActiveCommandBuffer;
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
		static VkRenderPass GetRenderPass();
		static const VkDevice& GetDevice();
		static const VkPhysicalDevice& GetPhysicalDevice();
		static void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		static void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
		static uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
		static VkCommandBuffer GetActiveCommandBuffer();
		static VkExtent2D GetSwapChainExtent();
		static VkSampleCountFlagBits GetSamples();
		static VkDescriptorPool GetDescriptorPool();
		

	private:
		static const int BUFFER_COUNT = 2;
		static API sRenderAPI;
		static GLFWwindow* sWindow;
		static std::vector<std::function<void()>> sRenderCommands;

		// Vulkan

		struct VulkanQueueFamilyIndices
		{
			VulkanQueueFamilyIndices() : GraphicsIndex(UINT32_MAX), PresentIndex(UINT32_MAX) {}

			uint32_t GraphicsIndex;
			uint32_t PresentIndex;

			bool HasPresentationQueue;

			bool IsComplete()
			{
				return GraphicsIndex != UINT32_MAX && PresentIndex != UINT32_MAX;
			}
		};

		struct VulkanSwapChainSupportDetails {
			VkSurfaceCapabilitiesKHR Capabilities;
			std::vector<VkSurfaceFormatKHR> Formats;
			std::vector<VkPresentModeKHR> PresentModes;
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
		static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		static VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		static VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
		static void CreateImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
		static VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
		static void CreateRenderPass();
		static VkFormat FindDepthFormat();
		static VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
		static void CreateDescriptorSetLayout();
		static void CreateFrameBuffers();
		static void CreateColorResource();
		static void CreateDepthResource();
		static void CreateCommandBuffer();
		static void CreateCommandPool();
		static void CreateSyncObjects();
		static VkCommandBuffer BeginSingleTimeCommands();
		static void EndSingleTimeCommands(VkCommandBuffer commandBuffer);
		static void RecreateSwapChain();
		static void DestroySwapChain();
		static void CreateDescriptorPool();
	};
	
}