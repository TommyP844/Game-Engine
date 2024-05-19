#pragma once

#include "Ref.h"
#include "Window.h"
#include "RenderContext.h"

#define PLATFORM_WIN32 1
#define VULKAN_SUPPORTED 1
#define DILIGENT_CPP_INTERFACE 1

#include "DiligentCore/Platforms/interface/PlatformDefinitions.h"
#include "DiligentCore/Graphics/GraphicsEngine/interface/EngineFactory.h"
#include "DiligentCore/Graphics/GraphicsEngine/interface/DeviceContext.h"
#include "DiligentCore/Graphics/GraphicsEngine/interface/GraphicsTypes.h"
#include "DiligentCore/Graphics/GraphicsAccessories/interface/GraphicsAccessories.hpp"
#include "DiligentCore/Graphics/GraphicsEngine/interface/SwapChain.h"

#include "DiligentTools/Imgui/interface/ImGuiImplDiligent.hpp"
#include "DiligentTools/Imgui/interface/ImGuiDiligentRenderer.hpp"
#include "DiligentTools/Imgui/interface/ImGuiImplWin32.hpp"

#include <map>
#include <thread>

namespace Mule
{
	enum class GraphicsAPI 
	{
		Vulkan
	};

	class GraphicsDevice
	{
	public:

		const uint32_t MAIN_THREAD_CTX_MASK = 0x01;
		const uint32_t ICON_THREAD_CTX_MASK = 0x01;

		static Ref<GraphicsDevice> Create(GraphicsAPI api, Ref<Window> window);

		void SwapBuffers();

		WeakRef<Diligent::IRenderDevice> GetRenderDevice();
		WeakRef<Diligent::ISwapChain> GetSwapChain() const { return mSwapChain.RawPtr(); }

		void ResizeSwapChain(int width, int height);

		RenderContext GetMainThreadContext();
		RenderContext GetSecondaryContext();

		~GraphicsDevice();
	private:
		GraphicsDevice(GraphicsAPI api, Ref<Window> window);

		Diligent::RefCntAutoPtr<Diligent::IEngineFactory> mFactory;
		Diligent::RefCntAutoPtr<Diligent::IRenderDevice> mDevice;
		Diligent::RefCntAutoPtr<Diligent::ISwapChain> mSwapChain;
		std::vector<Diligent::IDeviceContext*> mDeviceContextList;

		Diligent::RENDER_DEVICE_TYPE mDeviceType;
		Diligent::ADAPTER_TYPE mAdapterType;
		uint32_t mAdapterID;
		Diligent::GraphicsAdapterInfo mAdapterAttribs;
		Diligent::SwapChainDesc mSwapChainDesc;

		uint32_t FindAdapter(Diligent::IEngineFactory* pFactory, Diligent::Version GraphicsAPIVersion, Diligent::GraphicsAdapterInfo& AdapterAttribs);

	};
}
