#include "mulepch.h"
#include "Mule/Renderer/Renderer.h"
#include "imgui.h"
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include "Mule/bgfxImGui/imgui.h"
#include "backends/imgui_impl_glfw.h"
#include <glm/glm.hpp>

namespace Mule
{


	static Renderer::RenderData sRenderData;

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
		init.type = bgfx::RendererType::Enum::OpenGL;
		
		
		if (!bgfx::init(init))
			return false;

		bgfx::setDebug(BGFX_DEBUG_TEXT);

		bgfx::reset(size.x, size.y);
		bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);
		bgfx::setViewRect(0, 0, 0, size.x, size.y);
		
		imguiCreate(size.x, size.y);

		ImGui_ImplGlfw_InitForOther(window->GetWindowPtr(), true);

		return true;
	}
	void Renderer::Shutdown()
	{
		bgfx::shutdown();
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


	void Renderer::BeginFrame(const glm::mat4& view, const glm::mat4& proj, Ref<FrameBuffer> framebuffer)
	{
		sRenderData.View = view;
		sRenderData.Proj = proj;
		sRenderData.Framebuffer = framebuffer;
		bgfx::touch(0);
	}

	void Renderer::Render()
	{
		if (sRenderData.Framebuffer)
		{
			sRenderData.Framebuffer->SetActive();
		}

		// set view projection
		bgfx::setViewTransform(0, glm::value_ptr(sRenderData.View), glm::value_ptr(sRenderData.Proj));

		for (DrawData& data : sRenderData.DrawData)
		{
			switch (data.Type)
			{
			case Renderer::CommandType::Draw:
				if(data.BindFunc)
					data.BindFunc();

				bgfx::setTransform(data.ModelIndex);

				for (Ref<Mesh> mesh : data.Mesh->Meshes)
				{
					mesh->Submit();
					data.Shader->Draw();
				}
				break;
			case Renderer::CommandType::Compute:

				break;
			case Renderer::CommandType::Func:
				//data.BindFunc();
				break;
			}
		}

		if (sRenderData.Framebuffer)
		{
			sRenderData.Framebuffer->Render();
			FrameBuffer::SetMainViewport();
		}

		sRenderData.DrawData.clear();
		sRenderData.ModelTransforms.clear();
		sRenderData.Framebuffer = nullptr;
		
	}

	void Renderer::EndFrame()
	{
		bgfx::frame();
	}

	void Renderer::Submit(Ref<MeshCollection> mesh, Ref<Shader> shader, Ref<Material> material, const glm::vec3& worldPos, const glm::mat4& model, std::function<void()> func)
	{
		DrawData dd;
		dd.Type = Renderer::CommandType::Draw;
		dd.Mesh = mesh;
		dd.Shader = shader;
		dd.Material = material;
		dd.BindFunc = func;
		dd.ModelIndex = bgfx::setTransform(glm::value_ptr(model));


		sRenderData.DrawData.push_back(dd);
		sRenderData.ModelTransforms.push_back(model);
	}

	void Mule::Renderer::Submit(Ref<Shader> shader, std::function<void()> func)
	{
		DrawData dd;
		dd.Type = Renderer::CommandType::Compute;
		dd.Shader = shader;
		dd.BindFunc = func;

		sRenderData.DrawData.push_back(dd);
	}

	void Renderer::Submit(std::function<void()> func)
	{
		DrawData dd;
		dd.BindFunc = func;

		sRenderData.DrawData.push_back(dd);
	}

}


