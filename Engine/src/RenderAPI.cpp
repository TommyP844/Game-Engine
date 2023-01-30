#include "mulepch.h"

#include "Mule/Renderer/RenderAPI.h"
#include "Mule/Util/Log.h"

#include <GLFW/glfw3.h>

#include <set>

#include "imgui.h"

#include "../ImGui/backends/imgui_impl_glfw.h"
#include "../ImGui/backends/imgui_impl_vulkan.h"

#define VK_CHECK(x) if (x != VK_SUCCESS) { LOG_ERR(L"Vulkan Command Failed"); }

namespace Mule
{
    
    RenderAPI::API RenderAPI::sRenderAPI;
    GLFWwindow* RenderAPI::sWindow;
    VulkanData RenderAPI::sVulkanData;
    std::vector<std::function<void()>> RenderAPI::sRenderCommands;

	void RenderAPI::Init(API api, GLFWwindow* window)
	{
		sRenderAPI = api;
        sWindow = window;
		switch (sRenderAPI)
		{
		case API::NONE:
			LOG_ERR(L"No render api selected");
			return;
			break;
		case API::VULKAN:
			InitVulkan();
			break;
		}
	}
    
    void RenderAPI::Shutdown()
    {
        switch (sRenderAPI)
        {
        case API::NONE:
            LOG_ERR(L"No render api selected to shutdown");
            break;
        case API::VULKAN:
            ShutdownVulkan();
            break;
        }
    }

    VkRenderPass RenderAPI::GetRenderPass()
    {
        return sVulkanData.RenderPass;
    }

    void RenderAPI::BeginFrame()
    {
        VK_CHECK(vkWaitForFences(sVulkanData.LogicalDevice, 1, &sVulkanData.InFlightFences[sVulkanData.CurrentFrame], VK_TRUE, UINT64_MAX));
        
        VkResult result = vkAcquireNextImageKHR(
            sVulkanData.LogicalDevice,
            sVulkanData.SwapChain,
            UINT64_MAX, 
            sVulkanData.ImageAvailableSemaphores[sVulkanData.CurrentFrame],
            VK_NULL_HANDLE,
            &sVulkanData.ImageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            LOG_MSG(L"Resizing swapchain");
            RecreateSwapChain();
        }

        if (vkResetFences(sVulkanData.LogicalDevice, 1, &sVulkanData.InFlightFences[sVulkanData.CurrentFrame]) != VK_SUCCESS)
        {
            LOG_ERR(L"Failed to reset fences");
        }

        sVulkanData.ActiveCommandBuffer = sVulkanData.CommandBuffers[sVulkanData.CurrentFrame];
        VK_CHECK(vkResetCommandBuffer(sVulkanData.ActiveCommandBuffer, 0));
        
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(sVulkanData.ActiveCommandBuffer, &beginInfo) != VK_SUCCESS)
        {
            LOG_ERR(L"Failed to begin command buffer");
        }
        
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = sVulkanData.RenderPass;
        renderPassInfo.framebuffer = sVulkanData.SwapChainFramebuffers[sVulkanData.ImageIndex];
        renderPassInfo.renderArea.offset = VkOffset2D{ 0, 0 };
        renderPassInfo.renderArea.extent = sVulkanData.SwapChainExtent;

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = { 0.1f, 0.1f, 0.4f, 0.0f };
        clearValues[1].depthStencil = VkClearDepthStencilValue{ 1.0f, 0 };

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(sVulkanData.ActiveCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    }

    void RenderAPI::SubmitCommand(const std::function<void()>& command)
    {
        sRenderCommands.push_back(command);
    }

    void RenderAPI::EndFrame()
    {
        vkCmdEndRenderPass(sVulkanData.ActiveCommandBuffer);
        vkEndCommandBuffer(sVulkanData.ActiveCommandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        
        VkSemaphore waitSemaphores[] = { sVulkanData.ImageAvailableSemaphores[sVulkanData.CurrentFrame] };
        VkPipelineStageFlags waitStages[] = { VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &sVulkanData.CommandBuffers[sVulkanData.CurrentFrame];

        VkSemaphore signalSemaphores[] = { sVulkanData.RenderFinishedSemaphores[sVulkanData.CurrentFrame] };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        vkQueueSubmit(sVulkanData.GraphicsQueue, 1, &submitInfo, sVulkanData.InFlightFences[sVulkanData.CurrentFrame]);
        
    }

    void RenderAPI::ExecuteCommands()
    {
        for (auto& command : sRenderCommands)
        {
            command();
        }
        sRenderCommands.clear();
    }

    void RenderAPI::InitImGui()
    {
        switch (sRenderAPI)
        {
            case RenderAPI::API::VULKAN:
                InitImguiForVulkan();
            break;
        }
    }

    void RenderAPI::ShutdownImGui()
    {
        switch(sRenderAPI)
        {
        case API::VULKAN:
            vkDestroyDescriptorPool(sVulkanData.LogicalDevice, sVulkanData.ImguiPool, nullptr);
            ImGui_ImplVulkan_Shutdown();
            ImGui::DestroyContext();
            break;
        }
    }

    void RenderAPI::InitImguiForVulkan()
    {
        VkDescriptorPoolSize pool_sizes[] =
        {
            { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
        };
        

        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        pool_info.maxSets = 1000;
        pool_info.poolSizeCount = std::size(pool_sizes);
        pool_info.pPoolSizes = pool_sizes;

        vkCreateDescriptorPool(sVulkanData.LogicalDevice, &pool_info, nullptr, &sVulkanData.ImguiPool);

        // 2: initialize imgui library

        //this initializes the core structures of imgui
        ImGui::CreateContext();

        //this initializes imgui for GLFW
        ImGui_ImplGlfw_InitForVulkan(sWindow, true);

        //this initializes imgui for Vulkan
        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = sVulkanData.Instance;
        init_info.PhysicalDevice = sVulkanData.PhysicalDevice;
        init_info.Device = sVulkanData.LogicalDevice;
        init_info.Queue = sVulkanData.GraphicsQueue;
        init_info.DescriptorPool = sVulkanData.ImguiPool;
        init_info.MinImageCount = 3;
        init_info.ImageCount = 3;
        init_info.MSAASamples = (VkSampleCountFlagBits)sVulkanData.MSAASamples;

        ImGui_ImplVulkan_Init(&init_info, sVulkanData.RenderPass);

        //execute a gpu command to upload imgui font textures
        auto cmd = BeginSingleTimeCommands();
        ImGui_ImplVulkan_CreateFontsTexture(cmd);
        EndSingleTimeCommands(cmd);

        //clear font textures from cpu data
        ImGui_ImplVulkan_DestroyFontUploadObjects();
    }

    void RenderAPI::BeginImGuiFrame()
    {
        switch (sRenderAPI)
        {
        case API::VULKAN:
            ImGui_ImplVulkan_NewFrame();
            ImGui_ImplGlfw_NewFrame();

            break;
        }
        ImGui::NewFrame();
    }

    void RenderAPI::EndImGuiFrame()
    {
        ImGui::Render();
        switch (sRenderAPI)
        {
        case API::VULKAN:
            ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), sVulkanData.ActiveCommandBuffer);
            break;
        }
        ImGui::EndFrame();
    }

    void RenderAPI::SwapBuffers()
    {
        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        VkSemaphore signalSemaphores[] = { sVulkanData.RenderFinishedSemaphores[sVulkanData.CurrentFrame] };
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &sVulkanData.SwapChain;

        presentInfo.pImageIndices = &sVulkanData.ImageIndex;
        
        VkResult result = vkQueuePresentKHR(sVulkanData.PresentQueue, &presentInfo);
        if (result == VkResult::VK_ERROR_OUT_OF_DATE_KHR || result == VkResult::VK_SUBOPTIMAL_KHR)
        {
            RecreateSwapChain();
        }

        sVulkanData.CurrentFrame = (sVulkanData.CurrentFrame + 1) % BUFFER_COUNT;
    }

    int RenderAPI::GetCurrentSwapChainFrame()
    {
        return sVulkanData.CurrentFrame;
    }

    const VkDevice& RenderAPI::GetDevice()
    {
        return sVulkanData.LogicalDevice;
    }

    const VkPhysicalDevice& RenderAPI::GetPhysicalDevice()
    {
        return sVulkanData.PhysicalDevice;
    }

    void RenderAPI::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
    {
        VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

        VkBufferCopy copyRegion{};
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        EndSingleTimeCommands(commandBuffer);
    }

    void RenderAPI::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        vkCreateBuffer(sVulkanData.LogicalDevice, &bufferInfo, nullptr, &buffer);
        
        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(sVulkanData.LogicalDevice, buffer, &memRequirements);
        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

        vkAllocateMemory(sVulkanData.LogicalDevice, &allocInfo, nullptr, &bufferMemory);
        VkDeviceSize offset = 0;
        vkBindBufferMemory(sVulkanData.LogicalDevice, buffer, bufferMemory, offset);
    }
	
    // VULKAN
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
        
        switch (messageSeverity)
        {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            LOG_ERR(L"Validation Layer: {0}", pCallbackData->pMessage);
            break;
        //case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        //case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        //    LOG_MSG(L"Validation Layer: {0}", pCallbackData->pMessage);
        //    break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            LOG_WARN(L"Validation Layer: {0}", pCallbackData->pMessage);
            break;
        }

        return VK_FALSE;
    }

    void RenderAPI::InitVulkan()
	{
		CreateVulkanInstance();
        if (glfwCreateWindowSurface(sVulkanData.Instance, sWindow, nullptr, (VkSurfaceKHR*)&sVulkanData.Surface) != VK_SUCCESS)
        {
            LOG_ERR(L"Failed to create vulkan surface");
            assert(0);
        }
        PickPhysicalDevice();
        CreateLogicalDevice();
        CreateSwapChain();
        CreateRenderPass();
        CreateColorResource();
        CreateDepthResource();
        CreateFrameBuffers();
        CreateCommandPool();
        CreateCommandBuffer();
        CreateSyncObjects();
        CreateDescriptorPool();
	}

    void RenderAPI::ShutdownVulkan()
    {
        //for (auto& image : sVulkanData.SwapChainImages)
        //{
        //    sVulkanData.LogicalDevice.destroyImage(image);
        //}
        //
        for (auto& imageView : sVulkanData.SwapChainImageViews)
        {
            vkDestroyImageView(sVulkanData.LogicalDevice, imageView, nullptr);
        }

        vkDestroyImage(sVulkanData.LogicalDevice, sVulkanData.ColorImage, nullptr);
        vkDestroyImage(sVulkanData.LogicalDevice, sVulkanData.DepthImage, nullptr);

        vkDestroyImageView(sVulkanData.LogicalDevice, sVulkanData.ColorImageView, nullptr);
        vkDestroyImageView(sVulkanData.LogicalDevice, sVulkanData.DepthImageView, nullptr);

        vkFreeMemory(sVulkanData.LogicalDevice, sVulkanData.ColorImageMemory, nullptr);
        vkFreeMemory(sVulkanData.LogicalDevice, sVulkanData.DepthImageMemory, nullptr);

        for (auto& frameBuffer : sVulkanData.SwapChainFramebuffers)
        {
            vkDestroyFramebuffer(sVulkanData.LogicalDevice, frameBuffer, nullptr);
        }

        vkDestroyDescriptorSetLayout(sVulkanData.LogicalDevice, sVulkanData.DesciptorLayout, nullptr);
        vkDestroyRenderPass(sVulkanData.LogicalDevice, sVulkanData.RenderPass, nullptr);
        vkDestroySwapchainKHR(sVulkanData.LogicalDevice, sVulkanData.SwapChain, nullptr);
        vkDestroySurfaceKHR(sVulkanData.Instance, sVulkanData.Surface, nullptr);
        vkDestroyDevice(sVulkanData.LogicalDevice, nullptr);
        vkDestroyInstance(sVulkanData.Instance, nullptr);
    }

	void RenderAPI::CreateVulkanInstance()
	{
        VkApplicationInfo appInfo{};
        
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        auto extensions = GetRequiredVulkanExtensions();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

#ifdef DEBUG
        
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT; 
        debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
        debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugCreateInfo.pfnUserCallback = debugCallback; 
        
        auto validationLayers = GetValidationLayers();
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        createInfo.pNext = &debugCreateInfo;
#else
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
#endif

        if (vkCreateInstance(&createInfo, nullptr, &sVulkanData.Instance) != VkResult::VK_SUCCESS) {
            LOG_ERR(L"Failed to create vulkan instance");
            assert(0);
        }
	}
    
    std::vector<const char*> RenderAPI::GetRequiredVulkanExtensions()
    {
        uint32_t count;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&count);
        std::vector<const char*> extensions;
        for (int i = 0; i < count; i++)
        {
            extensions.push_back(glfwExtensions[i]);
        }

        // TODO: might want to put a macro block around this for debug only
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        return extensions;
    }

    std::vector<const char*> RenderAPI::GetRequiredDeviceExtensions()
    {
        return {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };
    }
    
    std::vector<const char*> RenderAPI::GetValidationLayers()
    {
        return {
            "VK_LAYER_KHRONOS_validation"
        };
    }
    
    void RenderAPI::PickPhysicalDevice()
    {
        uint32_t count;
        vkEnumeratePhysicalDevices(sVulkanData.Instance, &count, 0);
        std::vector<VkPhysicalDevice> devices(count);
        vkEnumeratePhysicalDevices(sVulkanData.Instance, &count, &devices[0]);
        LOG_MSG(L"Available devices");
        for (auto& device : devices)
        {
            VkPhysicalDeviceProperties props;
            vkGetPhysicalDeviceProperties(device, &props);
            
            LOG_MSG(L"\t Device: {0}", std::string(props.deviceName));
            
        }
        sVulkanData.PhysicalDevice = devices[0];

        if (!sVulkanData.PhysicalDevice)
        {
            LOG_ERR(L"Failed to pick a suitable device");
            assert(0);
        }
    }

    void RenderAPI::CreateLogicalDevice()
    {
        VulkanQueueFamilyIndices indices = FindQueueFamilies();

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = { indices.GraphicsIndex, indices.PresentIndex};

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};
        deviceFeatures.samplerAnisotropy = VK_TRUE;

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.pEnabledFeatures = &deviceFeatures;

        auto deviceExtensions = GetRequiredDeviceExtensions();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

#ifdef DEBUG
        auto validationLayers = GetValidationLayers();
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
#else
        createInfo.enabledLayerCount = 0;
#endif
        vkCreateDevice(sVulkanData.PhysicalDevice, &createInfo, nullptr, &sVulkanData.LogicalDevice);

        vkGetDeviceQueue(sVulkanData.LogicalDevice, indices.PresentIndex, 0, &sVulkanData.PresentQueue);
        vkGetDeviceQueue(sVulkanData.LogicalDevice, indices.GraphicsIndex, 0, &sVulkanData.GraphicsQueue);
    }

    RenderAPI::VulkanQueueFamilyIndices RenderAPI::FindQueueFamilies()
    {
        VulkanQueueFamilyIndices indices{};

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(sVulkanData.PhysicalDevice, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(sVulkanData.PhysicalDevice, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto& queueFamily : queueFamilies) {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.GraphicsIndex = i;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(sVulkanData.PhysicalDevice, i, sVulkanData.Surface, &presentSupport);

            if (presentSupport) {
                indices.PresentIndex = i;
            }

            if (indices.IsComplete()) {
                break;
            }

            i++;
        }

        return indices;
    }
    
    void RenderAPI::CreateSwapChain()
    {
        VulkanSwapChainSupportDetails swapChainSupport = QuerySwapChainSupportDetails();

        VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.Formats);
        VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.PresentModes);
        VkExtent2D extent = ChooseSwapExtent(swapChainSupport.Capabilities);

        uint32_t imageCount = swapChainSupport.Capabilities.minImageCount + 1;
        if (swapChainSupport.Capabilities.maxImageCount > 0 && imageCount > swapChainSupport.Capabilities.maxImageCount) {
            imageCount = swapChainSupport.Capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = sVulkanData.Surface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        VulkanQueueFamilyIndices indices = FindQueueFamilies();
        uint32_t queueFamilyIndices[] = { indices.GraphicsIndex, indices.PresentIndex };

        
        if (indices.GraphicsIndex != indices.PresentIndex) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        createInfo.preTransform = swapChainSupport.Capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;

        vkCreateSwapchainKHR(sVulkanData.LogicalDevice, &createInfo, nullptr, &sVulkanData.SwapChain);

        uint32_t swapChainImages;
        vkGetSwapchainImagesKHR(sVulkanData.LogicalDevice, sVulkanData.SwapChain, &swapChainImages, NULL);
        sVulkanData.SwapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(sVulkanData.LogicalDevice, sVulkanData.SwapChain, &swapChainImages, &sVulkanData.SwapChainImages[0]);

        sVulkanData.SwapChainFormat = surfaceFormat.format;
        sVulkanData.SwapChainExtent = extent;

        sVulkanData.SwapChainImageViews.resize(sVulkanData.SwapChainImages.size());

        for (uint32_t i = 0; i < sVulkanData.SwapChainImages.size(); i++)
        {
            sVulkanData.SwapChainImageViews[i] = CreateImageView(sVulkanData.SwapChainImages[i], sVulkanData.SwapChainFormat, VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT, 1);
        }
    }

    RenderAPI::VulkanSwapChainSupportDetails RenderAPI::QuerySwapChainSupportDetails()
    {
        VulkanSwapChainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(sVulkanData.PhysicalDevice, sVulkanData.Surface, &details.Capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(sVulkanData.PhysicalDevice, sVulkanData.Surface, &formatCount, nullptr);

        if (formatCount != 0) {
            details.Formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(sVulkanData.PhysicalDevice, sVulkanData.Surface, &formatCount, &details.Formats[0]);
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(sVulkanData.PhysicalDevice, sVulkanData.Surface, &presentModeCount, nullptr);

        if (presentModeCount != 0) {
            details.PresentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(sVulkanData.PhysicalDevice, sVulkanData.Surface, &presentModeCount, &details.PresentModes[0]);
        }

        return details;
    }

    VkSurfaceFormatKHR RenderAPI::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
    {
        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_R8G8B8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    VkPresentModeKHR RenderAPI::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
    {
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return availablePresentMode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D RenderAPI::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
    {
#ifdef max
#undef max
#endif
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        }
        else {
            int width, height;
            glfwGetFramebufferSize(sWindow, &width, &height);

            VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

    void RenderAPI::CreateImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
    {
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = mipLevels;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.samples = numSamples;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        vkCreateImage(sVulkanData.LogicalDevice, &imageInfo, nullptr, &image);

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(sVulkanData.LogicalDevice, image, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

        vkAllocateMemory(sVulkanData.LogicalDevice, &allocInfo, nullptr, &imageMemory);
        vkBindImageMemory(sVulkanData.LogicalDevice, image, imageMemory, 0);
    }
    
    VkImageView RenderAPI::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels)
    {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = format;
        viewInfo.subresourceRange.aspectMask = aspectFlags;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = mipLevels;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        VkImageView imageView;
        vkCreateImageView(sVulkanData.LogicalDevice, &viewInfo, nullptr, &imageView);

        return imageView;
    }
    
    void RenderAPI::CreateRenderPass()
    {
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = sVulkanData.SwapChainFormat;
        colorAttachment.samples = sVulkanData.MSAASamples;
        colorAttachment.loadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED; // undefined
        colorAttachment.finalLayout = VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // color optimal
                
        VkAttachmentDescription depthAttachment{};
        depthAttachment.format = FindDepthFormat();
        depthAttachment.samples = sVulkanData.MSAASamples;
        depthAttachment.loadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        
        VkAttachmentDescription colorAttachmentResolve{};
        colorAttachmentResolve.format = sVulkanData.SwapChainFormat;
        colorAttachmentResolve.samples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
        colorAttachmentResolve.loadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachmentResolve.storeOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachmentResolve.stencilLoadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachmentResolve.stencilStoreOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachmentResolve.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachmentResolve.finalLayout = VkImageLayout::VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        
        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        
        VkAttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference colorAttachmentResolveRef{};
        colorAttachmentResolveRef.attachment = 2;
        colorAttachmentResolveRef.layout = VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;
        subpass.pResolveAttachments = &colorAttachmentResolveRef;


        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = {};
        dependency.srcStageMask = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VkPipelineStageFlagBits::VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.srcAccessMask = {};
        dependency.dstStageMask = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT| VkPipelineStageFlagBits::VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstAccessMask = VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VkAccessFlagBits::VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        
        std::array<VkAttachmentDescription, 3> attachments = { colorAttachment, depthAttachment, colorAttachmentResolve };
        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        vkCreateRenderPass(sVulkanData.LogicalDevice, &renderPassInfo, nullptr, &sVulkanData.RenderPass);
    }
    
    VkFormat RenderAPI::FindDepthFormat()
    {
        return FindSupportedFormat(
            { VkFormat::VK_FORMAT_D32_SFLOAT, VkFormat::VK_FORMAT_D32_SFLOAT_S8_UINT, VkFormat::VK_FORMAT_D24_UNORM_S8_UINT},
            VkImageTiling::VK_IMAGE_TILING_OPTIMAL,
            VkFormatFeatureFlagBits::VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
        );
    }
    
    VkFormat RenderAPI::FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
    {
        for (VkFormat format : candidates) {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(sVulkanData.PhysicalDevice, format, &props);

            if (tiling == VkImageTiling::VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) 
            {
                return format;
            }
            else if (tiling == VkImageTiling::VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) 
            {
                return format;
            }
        }
        assert(0);
    }
    
    void RenderAPI::CreateDescriptorSetLayout()
    {
        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorCount = 1;
        //uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.pImmutableSamplers = nullptr;
        //uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutBinding samplerLayoutBinding{};
        samplerLayoutBinding.binding = 1;
        samplerLayoutBinding.descriptorCount = 1;
        samplerLayoutBinding.descriptorType = VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerLayoutBinding.pImmutableSamplers = nullptr;
        samplerLayoutBinding.stageFlags = VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT;
        
        std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };
        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        layoutInfo.pBindings = bindings.data();

        vkCreateDescriptorSetLayout(sVulkanData.LogicalDevice, &layoutInfo, nullptr, &sVulkanData.DesciptorLayout);
    }

    void RenderAPI::CreateFrameBuffers()
    {
        sVulkanData.SwapChainFramebuffers.resize(sVulkanData.SwapChainImageViews.size());

        for (size_t i = 0; i < sVulkanData.SwapChainFramebuffers.size(); i++) {
            std::array<VkImageView, 3> attachments = {
                sVulkanData.ColorImageView,
                sVulkanData.DepthImageView,
                sVulkanData.SwapChainImageViews[i]
            };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = sVulkanData.RenderPass;
            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebufferInfo.pAttachments = &attachments[0];
            framebufferInfo.width = sVulkanData.SwapChainExtent.width;
            framebufferInfo.height = sVulkanData.SwapChainExtent.height;
            framebufferInfo.layers = 1;

            vkCreateFramebuffer(sVulkanData.LogicalDevice, &framebufferInfo, nullptr, &sVulkanData.SwapChainFramebuffers[i]);
        }
    }

    void RenderAPI::CreateColorResource()
    {
        VkFormat colorFormat = sVulkanData.SwapChainFormat;
        VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        CreateImage(
            sVulkanData.SwapChainExtent.width, 
            sVulkanData.SwapChainExtent.height,
            1, 
            sVulkanData.MSAASamples, 
            colorFormat, 
            VkImageTiling::VK_IMAGE_TILING_OPTIMAL,
            VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            sVulkanData.ColorImage, 
            sVulkanData.ColorImageMemory);

        sVulkanData.ColorImageView = CreateImageView(
            sVulkanData.ColorImage, 
            colorFormat, 
            VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT, 
            1);
    }

    void RenderAPI::CreateDepthResource()
    {
        VkFormat depthFormat = FindDepthFormat();

        CreateImage(
            sVulkanData.SwapChainExtent.width,
            sVulkanData.SwapChainExtent.height,
            1,
            sVulkanData.MSAASamples,
            depthFormat,
            VkImageTiling::VK_IMAGE_TILING_OPTIMAL,
            VkImageUsageFlagBits::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
            VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            sVulkanData.DepthImage,
            sVulkanData.DepthImageMemory);

        sVulkanData.DepthImageView = CreateImageView(
            sVulkanData.DepthImage,
            depthFormat,
            VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT,
            1);
    }

    void RenderAPI::CreateCommandBuffer()
    {
        sVulkanData.CommandBuffers.reserve(BUFFER_COUNT);

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = sVulkanData.CommandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = BUFFER_COUNT;

        if (vkAllocateCommandBuffers(sVulkanData.LogicalDevice, &allocInfo, &sVulkanData.CommandBuffers[0]) != VK_SUCCESS)
        {
            LOG_ERR(L"Failed to allocate commandbuffers");
        }
    }

    void RenderAPI::CreateCommandPool()
    {
        VulkanQueueFamilyIndices queueFamilyIndices = FindQueueFamilies();
        
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = queueFamilyIndices.GraphicsIndex;

        vkCreateCommandPool(sVulkanData.LogicalDevice, &poolInfo, nullptr, &sVulkanData.CommandPool);
    }

    void RenderAPI::CreateSyncObjects()
    {
        sVulkanData.ImageAvailableSemaphores.resize(BUFFER_COUNT);
        sVulkanData.RenderFinishedSemaphores.resize(BUFFER_COUNT);
        sVulkanData.InFlightFences.resize(BUFFER_COUNT);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VkFenceCreateFlagBits::VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < BUFFER_COUNT; i++) 
        {
            vkCreateSemaphore(sVulkanData.LogicalDevice, &semaphoreInfo, nullptr, &sVulkanData.ImageAvailableSemaphores[i]);
            vkCreateSemaphore(sVulkanData.LogicalDevice, &semaphoreInfo, nullptr, &sVulkanData.RenderFinishedSemaphores[i]);
            vkCreateFence(sVulkanData.LogicalDevice, &fenceInfo, nullptr, &sVulkanData.InFlightFences[i]);
        }
    }

    VkCommandBuffer RenderAPI::BeginSingleTimeCommands()
    {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = sVulkanData.CommandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer cmd;
        vkAllocateCommandBuffers(sVulkanData.LogicalDevice, &allocInfo, &cmd);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        
        vkBeginCommandBuffer(cmd, &beginInfo);

        return cmd;
    }

    void RenderAPI::EndSingleTimeCommands(VkCommandBuffer commandBuffer)
    {
        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(sVulkanData.GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);

        vkQueueWaitIdle(sVulkanData.GraphicsQueue);

        vkFreeCommandBuffers(sVulkanData.LogicalDevice, sVulkanData.CommandPool, 1, &commandBuffer);
    }

    void RenderAPI::RecreateSwapChain()
    {
        int width = 0, height = 0;
        glfwGetFramebufferSize(sWindow, &width, &height);
        while (width == 0 || height == 0) {
            glfwGetFramebufferSize(sWindow, &width, &height);
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(sVulkanData.LogicalDevice);

        DestroySwapChain();

        CreateSwapChain();
        CreateColorResource();
        CreateDepthResource();
        CreateFrameBuffers();
    }

    void RenderAPI::DestroySwapChain()
    {
        vkDestroyImageView(sVulkanData.LogicalDevice, sVulkanData.DepthImageView, nullptr);
        vkDestroyImage(sVulkanData.LogicalDevice, sVulkanData.DepthImage, nullptr);
        vkFreeMemory(sVulkanData.LogicalDevice, sVulkanData.DepthImageMemory, nullptr);

        vkDestroyImageView(sVulkanData.LogicalDevice, sVulkanData.ColorImageView, nullptr);
        vkDestroyImage(sVulkanData.LogicalDevice, sVulkanData.ColorImage, nullptr);
        vkFreeMemory(sVulkanData.LogicalDevice, sVulkanData.ColorImageMemory, nullptr);

        for (auto& framebuffer : sVulkanData.SwapChainFramebuffers) 
        {
            vkDestroyFramebuffer(sVulkanData.LogicalDevice, framebuffer, nullptr);
        }

        for (auto& imageView : sVulkanData.SwapChainImageViews) 
        {
            vkDestroyImageView(sVulkanData.LogicalDevice, imageView, nullptr);
        }

        vkDestroySwapchainKHR(sVulkanData.LogicalDevice, sVulkanData.SwapChain, nullptr);
    }

    void RenderAPI::CreateDescriptorPool()
    {
        VkDescriptorPoolSize poolSize{};
        poolSize.type = VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSize.descriptorCount = static_cast<uint32_t>(BUFFER_COUNT);
        

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = 1;
        poolInfo.pPoolSizes = &poolSize;
        poolInfo.maxSets = static_cast<uint32_t>(BUFFER_COUNT);

        vkCreateDescriptorPool(sVulkanData.LogicalDevice, &poolInfo, nullptr, &sVulkanData.DescriptorPool);
    }

    uint32_t RenderAPI::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
    {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(sVulkanData.PhysicalDevice, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }
        LOG_ERR(L"VULKAN - Failed to find memory type");
        return 0;
    }
    
    VkCommandBuffer RenderAPI::GetActiveCommandBuffer()
    {
        return sVulkanData.ActiveCommandBuffer;
    }
    
    VkExtent2D RenderAPI::GetSwapChainExtent()
    {
        return sVulkanData.SwapChainExtent;
    }

    VkSampleCountFlagBits RenderAPI::GetSamples()
    {
        return sVulkanData.MSAASamples;
    }

    VkDescriptorPool RenderAPI::GetDescriptorPool()
    {
        return sVulkanData.DescriptorPool;
    }
}