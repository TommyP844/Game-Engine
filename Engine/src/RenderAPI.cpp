#include "mulepch.h"

#include "Mule/Renderer/RenderAPI.h"
#include "Mule/Util/Log.h"

#include <GLFW/glfw3.h>

#include <set>

#include "imgui.h"

#include "../ImGui/backends/imgui_impl_glfw.h"
#include "../ImGui/backends/imgui_impl_vulkan.h"

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

    vk::RenderPass RenderAPI::GetRenderPass()
    {
        return sVulkanData.RenderPass;
    }

    void RenderAPI::BeginFrame()
    {
        sVulkanData.LogicalDevice.waitForFences(sVulkanData.InFlightFences[sVulkanData.CurrentFrame], VK_TRUE, UINT64_MAX);
        
        auto res = sVulkanData.LogicalDevice.acquireNextImageKHR(sVulkanData.SwapChain, UINT64_MAX, sVulkanData.ImageAvailableSemaphores[sVulkanData.CurrentFrame]);
        sVulkanData.ImageIndex = res.value;
        if (res.result == vk::Result::eErrorOutOfDateKHR)
        {
            RecreateSwapChain();
        }


        sVulkanData.LogicalDevice.resetFences(sVulkanData.InFlightFences[sVulkanData.CurrentFrame]);

        sVulkanData.ActiveCommandBuffer = sVulkanData.CommandBuffers[sVulkanData.CurrentFrame];
        sVulkanData.ActiveCommandBuffer.reset();
        

        vk::CommandBufferBeginInfo beginInfo{};
        beginInfo.sType = vk::StructureType::eCommandBufferBeginInfo;

        sVulkanData.ActiveCommandBuffer.begin(beginInfo);

        vk::RenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = vk::StructureType::eRenderPassBeginInfo;
        renderPassInfo.renderPass = sVulkanData.RenderPass;
        renderPassInfo.framebuffer = sVulkanData.SwapChainFramebuffers[sVulkanData.ImageIndex];
        renderPassInfo.renderArea.offset = vk::Offset2D{ 0, 0 };
        renderPassInfo.renderArea.extent = sVulkanData.SwapChainExtent;

        std::array<vk::ClearValue, 2> clearValues{};
        std::array<float, 4> clearColor = { 0.1f, 0.1f, 0.4f, 0.0f };
        clearValues[0].color = vk::ClearColorValue(clearColor);
        clearValues[1].depthStencil = vk::ClearDepthStencilValue{ 1.0f, 0 };

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        sVulkanData.ActiveCommandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
    }

    void RenderAPI::SubmitCommand(const std::function<void()>& command)
    {
        sRenderCommands.push_back(command);
    }

    void RenderAPI::EndFrame()
    {
        sVulkanData.ActiveCommandBuffer.endRenderPass();
        sVulkanData.ActiveCommandBuffer.end();

        vk::SubmitInfo submitInfo{};
        submitInfo.sType = vk::StructureType::eSubmitInfo;
        
        vk::Semaphore waitSemaphores[] = { sVulkanData.ImageAvailableSemaphores[sVulkanData.CurrentFrame] };
        vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &sVulkanData.CommandBuffers[sVulkanData.CurrentFrame];

        vk::Semaphore signalSemaphores[] = { sVulkanData.RenderFinishedSemaphores[sVulkanData.CurrentFrame] };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        sVulkanData.GraphicsQueue.submit(submitInfo, sVulkanData.InFlightFences[sVulkanData.CurrentFrame]);

        
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
            sVulkanData.LogicalDevice.destroyDescriptorPool(sVulkanData.ImguiPool);
            ImGui_ImplVulkan_Shutdown();
            ImGui::DestroyContext();
            break;
        }
    }

    void RenderAPI::InitImguiForVulkan()
    {
        //1: create descriptor pool for IMGUI
    // the size of the pool is very oversize, but it's copied from imgui demo itself.
        vk::DescriptorPoolSize pool_sizes[] =
        {
            { vk::DescriptorType::eSampler, 1000 },
            { vk::DescriptorType::eCombinedImageSampler, 1000 },
            { vk::DescriptorType::eSampledImage, 1000 },
            { vk::DescriptorType::eStorageImage, 1000 },
            { vk::DescriptorType::eUniformTexelBuffer, 1000 },
            { vk::DescriptorType::eStorageTexelBuffer, 1000 },
            { vk::DescriptorType::eUniformBuffer, 1000 },
            { vk::DescriptorType::eStorageBuffer, 1000 },
            { vk::DescriptorType::eUniformBufferDynamic, 1000 },
            { vk::DescriptorType::eStorageBufferDynamic, 1000 },
            { vk::DescriptorType::eInputAttachment, 1000 }
        };
        

        vk::DescriptorPoolCreateInfo pool_info = {};
        pool_info.sType = vk::StructureType::eDescriptorPoolCreateInfo;
        pool_info.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
        pool_info.maxSets = 1000;
        pool_info.poolSizeCount = std::size(pool_sizes);
        pool_info.pPoolSizes = pool_sizes;

        sVulkanData.ImguiPool = sVulkanData.LogicalDevice.createDescriptorPool(pool_info);

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
        vk::PresentInfoKHR presentInfo{};
        presentInfo.sType = vk::StructureType::ePresentInfoKHR;

        vk::Semaphore signalSemaphores[] = { sVulkanData.RenderFinishedSemaphores[sVulkanData.CurrentFrame] };
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &sVulkanData.SwapChain;

        presentInfo.pImageIndices = &sVulkanData.ImageIndex;
        
        try
        {
            vk::Result result = sVulkanData.PresentQueue.presentKHR(presentInfo);
        }
        catch (vk::IncompatibleDisplayKHRError& e)
        {
            RecreateSwapChain();
        }

        sVulkanData.CurrentFrame = (sVulkanData.CurrentFrame + 1) % BUFFER_COUNT;
    }

    int RenderAPI::GetCurrentSwapChainFrame()
    {
        return sVulkanData.CurrentFrame;
    }

    const vk::Device& RenderAPI::GetDevice()
    {
        return sVulkanData.LogicalDevice;
    }

    const vk::PhysicalDevice& RenderAPI::GetPhysicalDevice()
    {
        return sVulkanData.PhysicalDevice;
    }

    void RenderAPI::CopyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size)
    {
        VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

        VkBufferCopy copyRegion{};
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        EndSingleTimeCommands(commandBuffer);
    }

    void RenderAPI::CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& bufferMemory)
    {
        vk::BufferCreateInfo bufferInfo{};
        bufferInfo.sType = vk::StructureType::eBufferCreateInfo;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = vk::SharingMode::eExclusive;

        buffer = sVulkanData.LogicalDevice.createBuffer(bufferInfo);
        
        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(sVulkanData.LogicalDevice, buffer, &memRequirements);
        vk::MemoryAllocateInfo allocInfo{};
        allocInfo.sType = vk::StructureType::eMemoryAllocateInfo;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

        bufferMemory = sVulkanData.LogicalDevice.allocateMemory(allocInfo);
        vk::DeviceSize offset = 0;
        sVulkanData.LogicalDevice.bindBufferMemory(buffer, bufferMemory, offset);
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
        //case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        //    LOG_WARN(L"Validation Layer: {0}", pCallbackData->pMessage);
        //    break;
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
            sVulkanData.LogicalDevice.destroyImageView(imageView);
        }

        sVulkanData.LogicalDevice.destroyImage(sVulkanData.ColorImage);
        sVulkanData.LogicalDevice.destroyImage(sVulkanData.DepthImage);

        sVulkanData.LogicalDevice.destroyImageView(sVulkanData.ColorImageView);
        sVulkanData.LogicalDevice.destroyImageView(sVulkanData.DepthImageView);

        sVulkanData.LogicalDevice.freeMemory(sVulkanData.ColorImageMemory);
        sVulkanData.LogicalDevice.freeMemory(sVulkanData.DepthImageMemory);

        for (auto& frameBuffer : sVulkanData.SwapChainFramebuffers)
        {
            sVulkanData.LogicalDevice.destroyFramebuffer(frameBuffer);
        }

        sVulkanData.LogicalDevice.destroyDescriptorSetLayout(sVulkanData.DesciptorLayout);
        sVulkanData.LogicalDevice.destroyRenderPass(sVulkanData.RenderPass);
        sVulkanData.LogicalDevice.destroySwapchainKHR(sVulkanData.SwapChain);
        sVulkanData.Instance.destroySurfaceKHR(sVulkanData.Surface);
        sVulkanData.LogicalDevice.destroy();
        sVulkanData.Instance.destroy();
    }

	void RenderAPI::CreateVulkanInstance()
	{
        vk::ApplicationInfo appInfo{};
        
        appInfo.sType = vk::StructureType::eApplicationInfo;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        vk::InstanceCreateInfo createInfo{};
        createInfo.sType = vk::StructureType::eInstanceCreateInfo;
        createInfo.pApplicationInfo = &appInfo;

        auto extensions = GetRequiredVulkanExtensions();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

#ifdef DEBUG

        vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        
        
        debugCreateInfo.sType = vk::StructureType::eDebugUtilsMessengerCreateInfoEXT;
        debugCreateInfo.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning;
        debugCreateInfo.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;
        debugCreateInfo.pfnUserCallback = debugCallback;
        
        auto validationLayers = GetValidationLayers();
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        createInfo.pNext = &debugCreateInfo;
#else
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
#endif

        if (vk::createInstance(&createInfo, nullptr, &sVulkanData.Instance) != vk::Result::eSuccess) {
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
        auto devices = sVulkanData.Instance.enumeratePhysicalDevices();
        LOG_MSG(L"Available devices");
        for (auto& device : devices)
        {
            auto props = device.getProperties();
            
            LOG_MSG(L"\t Device: {0}", std::string(props.deviceName.data()));
            
        }
        sVulkanData.PhysicalDevice = devices[0];
        LOG_MSG(L"Picking phyical device: {0}", std::string(sVulkanData.PhysicalDevice.getProperties().deviceName.data()));

        if (!sVulkanData.PhysicalDevice)
        {
            LOG_ERR(L"Failed to pick a suitable device");
            assert(0);
        }
    }

    void RenderAPI::CreateLogicalDevice()
    {
        VulkanQueueFamilyIndices indices = FindQueueFamilies();

        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = { indices.GraphicsIndex, indices.PresentIndex};

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) {
            vk::DeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = vk::StructureType::eDeviceQueueCreateInfo;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        vk::PhysicalDeviceFeatures deviceFeatures{};
        deviceFeatures.samplerAnisotropy = VK_TRUE;

        vk::DeviceCreateInfo createInfo{};
        createInfo.sType = vk::StructureType::eDeviceCreateInfo;
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
        try
        {
            sVulkanData.LogicalDevice = sVulkanData.PhysicalDevice.createDevice(createInfo);
        }
        catch (const vk::SystemError& err)
        {
            std::cout << err.what() << std::endl;
            assert(0);
        }

        try
        {
            sVulkanData.GraphicsQueue = sVulkanData.LogicalDevice.getQueue(indices.GraphicsIndex, 0);
            sVulkanData.PresentQueue = sVulkanData.LogicalDevice.getQueue(indices.PresentIndex, 0);
        }
        catch (const vk::SystemError& err)
        {
            std::cout << err.what() << std::endl;
            assert(0);
        }
    }

    RenderAPI::VulkanQueueFamilyIndices RenderAPI::FindQueueFamilies()
    {
        VulkanQueueFamilyIndices indices{};

        uint32_t queueFamilyCount = 0;
        auto queueFamilyProperties = sVulkanData.PhysicalDevice.getQueueFamilyProperties();
        
        int i = 0;
        for (const auto& queueFamily : queueFamilyProperties) 
        {
            if (queueFamily.queueFlags & vk::QueueFlags::Flags(VK_QUEUE_GRAPHICS_BIT))
            {
                indices.GraphicsIndex = i;
            }

            uint32_t presentIndex = sVulkanData.PhysicalDevice.getSurfaceSupportKHR(i, sVulkanData.Surface);
            
            if (presentIndex == VK_TRUE) {
                indices.PresentIndex = i;
            }

            if (indices.GraphicsIndex && indices.PresentIndex) {

                break;
            }

            i++;
        }

        return indices;
    }
    
    void RenderAPI::CreateSwapChain()
    {
        VulkanSwapChainSupportDetails swapChainSupport = QuerySwapChainSupportDetails();

        vk::SurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.Formats);
        vk::PresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.PresentModes);
        vk::Extent2D extent = ChooseSwapExtent(swapChainSupport.Capabilities);

        uint32_t imageCount = swapChainSupport.Capabilities.minImageCount + 1;
        if (swapChainSupport.Capabilities.maxImageCount > 0 && imageCount > swapChainSupport.Capabilities.maxImageCount) {
            imageCount = swapChainSupport.Capabilities.maxImageCount;
        }

        vk::SwapchainCreateInfoKHR createInfo{};
        createInfo.sType = vk::StructureType::eSwapchainCreateInfoKHR;
        createInfo.surface = sVulkanData.Surface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

        VulkanQueueFamilyIndices indices = FindQueueFamilies();
        uint32_t queueFamilyIndices[] = { indices.GraphicsIndex, indices.PresentIndex };

        
        if (indices.GraphicsIndex != indices.PresentIndex) {
            createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else {
            createInfo.imageSharingMode = vk::SharingMode::eExclusive;
        }

        createInfo.preTransform = swapChainSupport.Capabilities.currentTransform;
        createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;

        try
        {
            sVulkanData.SwapChain = sVulkanData.LogicalDevice.createSwapchainKHR(createInfo);
        }
        catch (const vk::SystemError& err)
        {
            std::cerr << err.what() << std::endl;
            assert(0);
        }
        sVulkanData.SwapChainImages = sVulkanData.LogicalDevice.getSwapchainImagesKHR(sVulkanData.SwapChain);

        sVulkanData.SwapChainFormat = surfaceFormat.format;
        sVulkanData.SwapChainExtent = extent;

        sVulkanData.SwapChainImageViews.resize(sVulkanData.SwapChainImages.size());

        for (uint32_t i = 0; i < sVulkanData.SwapChainImages.size(); i++)
        {
            sVulkanData.SwapChainImageViews[i] = CreateImageView(sVulkanData.SwapChainImages[i], sVulkanData.SwapChainFormat, vk::ImageAspectFlagBits::eColor, 1);
        }
    }

    RenderAPI::VulkanSwapChainSupportDetails RenderAPI::QuerySwapChainSupportDetails()
    {
        VulkanSwapChainSupportDetails details;

        details.Capabilities = sVulkanData.PhysicalDevice.getSurfaceCapabilitiesKHR(sVulkanData.Surface);    
        details.Formats = sVulkanData.PhysicalDevice.getSurfaceFormatsKHR(sVulkanData.Surface);
        details.PresentModes = sVulkanData.PhysicalDevice.getSurfacePresentModesKHR(sVulkanData.Surface);

        return details;
    }

    vk::SurfaceFormatKHR RenderAPI::ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats)
    {
        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == vk::Format::eR8G8B8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
            {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    vk::PresentModeKHR RenderAPI::ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes)
    {
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
                return availablePresentMode;
            }
        }

        return vk::PresentModeKHR::eFifo;
    }

    vk::Extent2D RenderAPI::ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities)
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

    void RenderAPI::CreateImage(uint32_t width, uint32_t height, uint32_t mipLevels, vk::SampleCountFlagBits numSamples, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Image& image, vk::DeviceMemory& imageMemory)
    {
        vk::ImageCreateInfo imageInfo{};
        imageInfo.sType = vk::StructureType::eImageCreateInfo;
        imageInfo.imageType = vk::ImageType::e2D;
        imageInfo.extent.width = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = mipLevels;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = vk::ImageLayout::eUndefined;
        imageInfo.usage = usage;
        imageInfo.samples = numSamples;
        imageInfo.sharingMode = vk::SharingMode::eExclusive;

        image = sVulkanData.LogicalDevice.createImage(imageInfo);

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(sVulkanData.LogicalDevice, image, &memRequirements);

        vk::MemoryAllocateInfo allocInfo{};
        allocInfo.sType = vk::StructureType::eMemoryAllocateInfo;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

        imageMemory = sVulkanData.LogicalDevice.allocateMemory(allocInfo);
        sVulkanData.LogicalDevice.bindImageMemory(image, imageMemory, vk::DeviceSize(0));

    }
    
    vk::ImageView RenderAPI::CreateImageView(vk::Image image, vk::Format format, vk::ImageAspectFlags aspectFlags, uint32_t mipLevels)
    {
        vk::ImageViewCreateInfo viewInfo{};
        viewInfo.sType = vk::StructureType::eImageViewCreateInfo;
        viewInfo.image = image;
        viewInfo.viewType = vk::ImageViewType::e2D;
        viewInfo.format = format;
        viewInfo.subresourceRange.aspectMask = aspectFlags;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = mipLevels;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        vk::ImageView imageView = sVulkanData.LogicalDevice.createImageView(viewInfo);

        return imageView;
    }
    
    void RenderAPI::CreateRenderPass()
    {
        vk::AttachmentDescription colorAttachment{};
        colorAttachment.format = sVulkanData.SwapChainFormat;
        colorAttachment.samples = sVulkanData.MSAASamples;
        colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
        colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
        colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        colorAttachment.initialLayout = vk::ImageLayout::eUndefined; // undefined
        colorAttachment.finalLayout = vk::ImageLayout::eColorAttachmentOptimal; // color optimal
                
        vk::AttachmentDescription depthAttachment{};
        depthAttachment.format = FindDepthFormat();
        depthAttachment.samples = sVulkanData.MSAASamples;
        depthAttachment.loadOp = vk::AttachmentLoadOp::eClear;
        depthAttachment.storeOp = vk::AttachmentStoreOp::eDontCare;
        depthAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        depthAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        depthAttachment.initialLayout = vk::ImageLayout::eUndefined;
        depthAttachment.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
        
        vk::AttachmentDescription colorAttachmentResolve{};
        colorAttachmentResolve.format = sVulkanData.SwapChainFormat;
        colorAttachmentResolve.samples = vk::SampleCountFlagBits::e1;
        colorAttachmentResolve.loadOp = vk::AttachmentLoadOp::eDontCare;
        colorAttachmentResolve.storeOp = vk::AttachmentStoreOp::eStore;
        colorAttachmentResolve.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        colorAttachmentResolve.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        colorAttachmentResolve.initialLayout = vk::ImageLayout::eUndefined;
        colorAttachmentResolve.finalLayout = vk::ImageLayout::ePresentSrcKHR;
        
        vk::AttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;
        
        vk::AttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

        vk::AttachmentReference colorAttachmentResolveRef{};
        colorAttachmentResolveRef.attachment = 2;
        colorAttachmentResolveRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

        vk::SubpassDescription subpass{};
        subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;
        subpass.pResolveAttachments = &colorAttachmentResolveRef;


        vk::SubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = {};
        dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
        dependency.srcAccessMask = {};
        dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
        dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
        
        std::array<vk::AttachmentDescription, 3> attachments = { colorAttachment, depthAttachment, colorAttachmentResolve };
        vk::RenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = vk::StructureType::eRenderPassCreateInfo;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        sVulkanData.RenderPass = sVulkanData.LogicalDevice.createRenderPass(renderPassInfo);
    }
    
    vk::Format RenderAPI::FindDepthFormat()
    {
        return FindSupportedFormat(
            { vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint },
            vk::ImageTiling::eOptimal,
            vk::FormatFeatureFlagBits::eDepthStencilAttachment
        );
    }
    
    vk::Format RenderAPI::FindSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features)
    {
        for (vk::Format format : candidates) {
            vk::FormatProperties props = sVulkanData.PhysicalDevice.getFormatProperties(format);
            
            if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features) 
            {
                return format;
            }
            else if (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features) 
            {
                return format;
            }
        }
        assert(0);
    }
    
    void RenderAPI::CreateDescriptorSetLayout()
    {
        vk::DescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorCount = 1;
        //uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.pImmutableSamplers = nullptr;
        //uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        vk::DescriptorSetLayoutBinding samplerLayoutBinding{};
        samplerLayoutBinding.binding = 1;
        samplerLayoutBinding.descriptorCount = 1;
        samplerLayoutBinding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
        samplerLayoutBinding.pImmutableSamplers = nullptr;
        samplerLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eFragment;
        

        std::array<vk::DescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };
        vk::DescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = vk::StructureType::eDescriptorSetLayoutCreateInfo;
        layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        layoutInfo.pBindings = bindings.data();

        sVulkanData.DesciptorLayout = sVulkanData.LogicalDevice.createDescriptorSetLayout(layoutInfo);
    }

    void RenderAPI::CreateFrameBuffers()
    {
        sVulkanData.SwapChainFramebuffers.resize(sVulkanData.SwapChainImageViews.size());

        for (size_t i = 0; i < sVulkanData.SwapChainFramebuffers.size(); i++) {
            std::array<vk::ImageView, 3> attachments = {
                sVulkanData.ColorImageView,
                sVulkanData.DepthImageView,
                sVulkanData.SwapChainImageViews[i]
            };

            vk::FramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = vk::StructureType::eFramebufferCreateInfo;
            framebufferInfo.renderPass = sVulkanData.RenderPass;
            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebufferInfo.pAttachments = &attachments[0];
            framebufferInfo.width = sVulkanData.SwapChainExtent.width;
            framebufferInfo.height = sVulkanData.SwapChainExtent.height;
            framebufferInfo.layers = 1;

            sVulkanData.SwapChainFramebuffers[i] = sVulkanData.LogicalDevice.createFramebuffer(framebufferInfo);
        }
    }

    void RenderAPI::CreateColorResource()
    {
        vk::Format colorFormat = sVulkanData.SwapChainFormat;
        vk::MemoryPropertyFlagBits::eDeviceLocal;
        CreateImage(
            sVulkanData.SwapChainExtent.width, 
            sVulkanData.SwapChainExtent.height,
            1, 
            sVulkanData.MSAASamples, 
            colorFormat, 
            vk::ImageTiling::eOptimal,
            vk::ImageUsageFlagBits::eTransientAttachment | vk::ImageUsageFlagBits::eColorAttachment,
            vk::MemoryPropertyFlagBits::eDeviceLocal,
            sVulkanData.ColorImage, 
            sVulkanData.ColorImageMemory);

        sVulkanData.ColorImageView = CreateImageView(
            sVulkanData.ColorImage, 
            colorFormat, 
            vk::ImageAspectFlagBits::eColor, 
            1);
    }

    void RenderAPI::CreateDepthResource()
    {
        vk::Format depthFormat = FindDepthFormat();

        CreateImage(
            sVulkanData.SwapChainExtent.width,
            sVulkanData.SwapChainExtent.height,
            1,
            sVulkanData.MSAASamples,
            depthFormat,
            vk::ImageTiling::eOptimal,
            vk::ImageUsageFlagBits::eDepthStencilAttachment,
            vk::MemoryPropertyFlagBits::eDeviceLocal,
            sVulkanData.DepthImage,
            sVulkanData.DepthImageMemory);

        sVulkanData.DepthImageView = CreateImageView(
            sVulkanData.DepthImage,
            depthFormat,
            vk::ImageAspectFlagBits::eDepth,
            1);
    }

    void RenderAPI::CreateCommandBuffer()
    {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = sVulkanData.CommandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = BUFFER_COUNT;

        sVulkanData.CommandBuffers = sVulkanData.LogicalDevice.allocateCommandBuffers(allocInfo);
    }

    void RenderAPI::CreateCommandPool()
    {
        VulkanQueueFamilyIndices queueFamilyIndices = FindQueueFamilies();
        
        vk::CommandPoolCreateInfo poolInfo{};
        poolInfo.sType = vk::StructureType::eCommandPoolCreateInfo;
        poolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;;
        poolInfo.queueFamilyIndex = queueFamilyIndices.GraphicsIndex;

        sVulkanData.CommandPool = sVulkanData.LogicalDevice.createCommandPool(poolInfo);
    }

    void RenderAPI::CreateSyncObjects()
    {
        sVulkanData.ImageAvailableSemaphores.resize(BUFFER_COUNT);
        sVulkanData.RenderFinishedSemaphores.resize(BUFFER_COUNT);
        sVulkanData.InFlightFences.resize(BUFFER_COUNT);

        vk::SemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = vk::StructureType::eSemaphoreCreateInfo;

        vk::FenceCreateInfo fenceInfo{};
        fenceInfo.sType = vk::StructureType::eFenceCreateInfo;
        fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;

        for (size_t i = 0; i < BUFFER_COUNT; i++) 
        {
            sVulkanData.ImageAvailableSemaphores[i] = sVulkanData.LogicalDevice.createSemaphore(semaphoreInfo);
            sVulkanData.RenderFinishedSemaphores[i] = sVulkanData.LogicalDevice.createSemaphore(semaphoreInfo);
            sVulkanData.InFlightFences[i] = sVulkanData.LogicalDevice.createFence(fenceInfo);
        }
    }

    VkCommandBuffer RenderAPI::BeginSingleTimeCommands()
    {
        vk::CommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = vk::StructureType::eCommandBufferAllocateInfo;
        allocInfo.level = vk::CommandBufferLevel::ePrimary;
        allocInfo.commandPool = sVulkanData.CommandPool;
        allocInfo.commandBufferCount = 1;

        auto commandBuffer = sVulkanData.LogicalDevice.allocateCommandBuffers(allocInfo);

        vk::CommandBufferBeginInfo beginInfo{};
        beginInfo.sType = vk::StructureType::eCommandBufferBeginInfo;
        beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
        
        commandBuffer[0].begin(beginInfo);

        return commandBuffer[0];
    }

    void RenderAPI::EndSingleTimeCommands(vk::CommandBuffer commandBuffer)
    {
        vkEndCommandBuffer(commandBuffer);

        vk::SubmitInfo submitInfo{};
        submitInfo.sType = vk::StructureType::eSubmitInfo;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        sVulkanData.GraphicsQueue.submit(submitInfo);
        sVulkanData.GraphicsQueue.waitIdle();

        sVulkanData.LogicalDevice.freeCommandBuffers(sVulkanData.CommandPool, commandBuffer);
    }

    void RenderAPI::RecreateSwapChain()
    {
        int width = 0, height = 0;
        glfwGetFramebufferSize(sWindow, &width, &height);
        while (width == 0 || height == 0) {
            glfwGetFramebufferSize(sWindow, &width, &height);
            glfwWaitEvents();
        }
        sVulkanData.LogicalDevice.waitIdle();
        

        DestroySwapChain();

        CreateSwapChain();
        CreateColorResource();
        CreateDepthResource();
        CreateFrameBuffers();
    }

    void RenderAPI::DestroySwapChain()
    {
        sVulkanData.LogicalDevice.destroyImageView(sVulkanData.DepthImageView);
        sVulkanData.LogicalDevice.destroyImage(sVulkanData.DepthImage);
        sVulkanData.LogicalDevice.freeMemory(sVulkanData.DepthImageMemory);

        sVulkanData.LogicalDevice.destroyImageView(sVulkanData.ColorImageView);
        sVulkanData.LogicalDevice.destroyImage(sVulkanData.ColorImage);
        sVulkanData.LogicalDevice.freeMemory(sVulkanData.ColorImageMemory);


        for (auto& framebuffer : sVulkanData.SwapChainFramebuffers) 
        {
            sVulkanData.LogicalDevice.destroyFramebuffer(framebuffer);
        }

        for (auto& imageView : sVulkanData.SwapChainImageViews) 
        {
            sVulkanData.LogicalDevice.destroyImageView(imageView);
        }

        sVulkanData.LogicalDevice.destroySwapchainKHR(sVulkanData.SwapChain);
    }

    void RenderAPI::CreateDescriptorPool()
    {
        vk::DescriptorPoolSize poolSize{};
        poolSize.type = vk::DescriptorType::eUniformBuffer;
        poolSize.descriptorCount = static_cast<uint32_t>(BUFFER_COUNT);
        

        vk::DescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = vk::StructureType::eDescriptorPoolCreateInfo;
        poolInfo.poolSizeCount = 1;
        poolInfo.pPoolSizes = &poolSize;
        poolInfo.maxSets = static_cast<uint32_t>(BUFFER_COUNT);

        sVulkanData.DescriptorPool = sVulkanData.LogicalDevice.createDescriptorPool(poolInfo);
    }

    uint32_t RenderAPI::FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties)
    {
        vk::PhysicalDeviceMemoryProperties memProperties = sVulkanData.PhysicalDevice.getMemoryProperties();

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }
        LOG_ERR(L"VULKAN - Failed to find memory type");
        return 0;
    }
    
    vk::CommandBuffer RenderAPI::GetActiveCommandBuffer()
    {
        return sVulkanData.ActiveCommandBuffer;
    }
    
    vk::Extent2D RenderAPI::GetSwapChainExtent()
    {
        return sVulkanData.SwapChainExtent;
    }

    vk::SampleCountFlagBits RenderAPI::GetSamples()
    {
        return sVulkanData.MSAASamples;
    }
    vk::DescriptorPool RenderAPI::GetDescriptorPool()
    {
        return sVulkanData.DescriptorPool;
    }
}