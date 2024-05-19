#include "Graphics/GraphicsDevice.h"

#include <vulkan/vulkan.h>
#include "DiligentCore/Graphics/GraphicsEngineVulkan/interface/EngineFactoryVk.h"
#include "DiligentCore/Graphics/GraphicsEngineVulkan/interface/RenderDeviceVk.h"
#include "DiligentCore/Graphics/GraphicsEngineVulkan/interface/DeviceContextVk.h"
#include "DiligentCore/Graphics/GraphicsEngineVulkan/interface/SwapChainVk.h"

#include "DiligentTools/Imgui/interface/ImGuiImplDiligent.hpp"

#include <glm.hpp>

#ifdef max
#undef max
#endif

using namespace Diligent;

namespace Mule
{
	Ref<GraphicsDevice> GraphicsDevice::Create(GraphicsAPI api, Ref<Window> window)
	{
		return Ref<GraphicsDevice>(new GraphicsDevice(api, window));
	}

    void GraphicsDevice::SwapBuffers()
    {
        mSwapChain->Present();

        const glm::vec4 clearColor = { 0, 0, 0, 0 };
        ITextureView* views = mSwapChain->GetCurrentBackBufferRTV();
        mDeviceContextList[0]->SetRenderTargets(1, &views, mSwapChain->GetDepthBufferDSV(), Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        mDeviceContextList[0]->ClearRenderTarget(mSwapChain->GetCurrentBackBufferRTV(), &clearColor[0], Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    }

    void GraphicsDevice::ResizeSwapChain(int width, int height)
    {
        mSwapChain->Resize(width, height);
    }

    RenderContext GraphicsDevice::GetMainThreadContext()
    {
        return RenderContext(mDeviceContextList[0], MAIN_THREAD_CTX_MASK);
    }

    RenderContext GraphicsDevice::GetSecondaryContext()
    {
        return RenderContext(mDeviceContextList[1], ICON_THREAD_CTX_MASK);
    }

    WeakRef<Diligent::IRenderDevice> GraphicsDevice::GetRenderDevice()
    {
        return WeakRef<Diligent::IRenderDevice>(mDevice.RawPtr());
    }

	GraphicsDevice::~GraphicsDevice()
	{
	}

    GraphicsDevice::GraphicsDevice(GraphicsAPI api, Ref<Window> window)
    {
        switch (api)
        {
        case Mule::GraphicsAPI::Vulkan:
        {
            mDeviceType = RENDER_DEVICE_TYPE_VULKAN;
            auto* pFactoryVk = Diligent::GetEngineFactoryVk();
            EngineVkCreateInfo EngineCI;

            EngineCI.SetValidationLevel(VALIDATION_LEVEL_2);

            const char* const ppIgnoreDebugMessages[] = //
            {
                // Validation Performance Warning: [ UNASSIGNED-CoreValidation-Shader-OutputNotConsumed ]
                // vertex shader writes to output location 1.0 which is not consumed by fragment shader
                "UNASSIGNED-CoreValidation-Shader-OutputNotConsumed",
                "WARNING-Shader-OutputNotConsumed",
            };
            EngineCI.ppIgnoreDebugMessageNames = ppIgnoreDebugMessages;
            EngineCI.IgnoreDebugMessageCount = _countof(ppIgnoreDebugMessages);
            EngineCI.AdapterId = FindAdapter(pFactoryVk, EngineCI.GraphicsAPIVersion, mAdapterAttribs);
            EngineCI.Features = DeviceFeatures{ DEVICE_FEATURE_STATE_OPTIONAL };
            EngineCI.Features.TransferQueueTimestampQueries = DEVICE_FEATURE_STATE_ENABLED;
            EngineCI.NumDeferredContexts = 2;

            mFactory = pFactoryVk;

            uint32_t NumImmediateContexts = std::max(1u, EngineCI.NumImmediateContexts);
            mDeviceContextList.resize(NumImmediateContexts + EngineCI.NumDeferredContexts);
            pFactoryVk->CreateDeviceAndContextsVk(EngineCI, &mDevice, mDeviceContextList.data());


            if (!mDevice)
            {
                LOG_ERROR_AND_THROW("Unable to initialize Diligent Engine in Vulkan mode. The API may not be available, "
                    "or required features may not be supported by this GPU/driver/OS version.");
            }
            NativeWindow win32Window;
            win32Window.hWnd = window->GetWin32Window();
            pFactoryVk->CreateSwapChainVk(mDevice, mDeviceContextList[0], mSwapChainDesc, win32Window, &mSwapChain);
        }
        break;
        default:
            break;
        }
	}

    uint32_t GraphicsDevice::FindAdapter(IEngineFactory* pFactory, Version GraphicsAPIVersion, GraphicsAdapterInfo& AdapterAttribs) {
        Uint32 NumAdapters = 0;
        pFactory->EnumerateAdapters(GraphicsAPIVersion, NumAdapters, nullptr);
        std::vector<GraphicsAdapterInfo> Adapters(NumAdapters);
        if (NumAdapters > 0)
            pFactory->EnumerateAdapters(GraphicsAPIVersion, NumAdapters, Adapters.data());
        else
            LOG_ERROR_AND_THROW("Failed to find compatible hardware adapters");

        auto AdapterId = mAdapterID;
        if (AdapterId != DEFAULT_ADAPTER_ID)
        {
            if (AdapterId < Adapters.size())
            {
                mAdapterType = Adapters[AdapterId].Type;
            }
            else
            {
                LOG_ERROR_MESSAGE("Adapter ID (", AdapterId, ") is invalid. Only ", Adapters.size(), " compatible ", (Adapters.size() == 1 ? "adapter" : "adapters"), " present in the system");
                AdapterId = DEFAULT_ADAPTER_ID;
            }
        }

        if (AdapterId == DEFAULT_ADAPTER_ID && mAdapterType != ADAPTER_TYPE_UNKNOWN)
        {
            for (Uint32 i = 0; i < Adapters.size(); ++i)
            {
                if (Adapters[i].Type == mAdapterType)
                {
                    AdapterId = i;
                    break;
                }
            }
            if (AdapterId == DEFAULT_ADAPTER_ID)
                LOG_WARNING_MESSAGE("Unable to find the requested adapter type. Using default adapter.");
        }

        if (AdapterId == DEFAULT_ADAPTER_ID)
        {
            mAdapterType = ADAPTER_TYPE_UNKNOWN;
            for (Uint32 i = 0; i < Adapters.size(); ++i)
            {
                const auto& AdapterInfo = Adapters[i];
                const auto  AdapterType = AdapterInfo.Type;
                static_assert((ADAPTER_TYPE_DISCRETE > ADAPTER_TYPE_INTEGRATED &&
                    ADAPTER_TYPE_INTEGRATED > ADAPTER_TYPE_SOFTWARE &&
                    ADAPTER_TYPE_SOFTWARE > ADAPTER_TYPE_UNKNOWN),
                    "Unexpected ADAPTER_TYPE enum ordering");
                if (AdapterType > mAdapterType)
                {
                    // Prefer Discrete over Integrated over Software
                    mAdapterType = AdapterType;
                    AdapterId = i;
                }
                else if (AdapterType == mAdapterType)
                {
                    // Select adapter with more memory
                    const auto& NewAdapterMem = AdapterInfo.Memory;
                    const auto  NewTotalMemory = NewAdapterMem.LocalMemory + NewAdapterMem.HostVisibleMemory + NewAdapterMem.UnifiedMemory;
                    const auto& CurrAdapterMem = Adapters[AdapterId].Memory;
                    const auto  CurrTotalMemory = CurrAdapterMem.LocalMemory + CurrAdapterMem.HostVisibleMemory + CurrAdapterMem.UnifiedMemory;
                    if (NewTotalMemory > CurrTotalMemory)
                    {
                        AdapterId = i;
                    }
                }
            }
        }

        if (AdapterId != DEFAULT_ADAPTER_ID)
        {
            AdapterAttribs = Adapters[AdapterId];
            LOG_INFO_MESSAGE("Using adapter ", AdapterId, ": '", AdapterAttribs.Description, "'");
        }

        return AdapterId;
    }
}