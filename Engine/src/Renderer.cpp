#include "mulepch.h"
#include "Mule/Renderer/Renderer.h"
#include "imgui.h"
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include "Mule/bgfxImGui/imgui.h"
#include "backends/imgui_impl_glfw.h"

namespace Mule
{
	struct RenderData
	{
		Ref<Window> Window;
	};

	static RenderData sRenderData;

	bool Renderer::Init(Ref<Window> window)
	{
		sRenderData.Window = window;

		bgfx::Init init;

		init.platformData.nwh = window->GetWin32Handle();

		glm::ivec2 size = window->GetSize();
		init.resolution.width = (uint32_t)size.x;
		init.resolution.height = (uint32_t)size.y;
		init.resolution.reset = BGFX_RESET_VSYNC;
		init.vendorId = BGFX_PCI_ID_NVIDIA;
		
		
		if (!bgfx::init(init))
			return false;

		bgfx::setViewClear(0
			, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH
			, 0x303030ff
			, 1.0f
			, 0
		);

		bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);
		bgfx::setViewRect(0, 0, 0, size.x, size.y);

		
		imguiCreate(size.x, size.y);

		ImGui_ImplGlfw_InitForOther(window->GetWindowPtr(), true);

		return true;
	}
	
	void Renderer::NewImGuiFrame()
	{
		glm::ivec2 size = sRenderData.Window->GetSize();
		glm::ivec2 cursorPos = sRenderData.Window->GetCursorPos();
		
		WindowEvent event = sRenderData.Window->GetEventAndPop();


		imguiBeginFrame();

	}
	void Renderer::EndImGuiFrame()
	{
		imguiEndFrame();		
	}

	void Renderer::AdvanceFrame()
	{
		bgfx::frame();
		bgfx::touch(0);
	}

	void Renderer::Shutdown()
	{
		bgfx::shutdown();
	}
}


