#include "Graphics/ImGuiMuleContext.h"


#include <glm.hpp>

namespace Mule
{
	Ref<ImGuiMuleContext> ImGuiMuleContext::Create(const ImGuiMuleCreateInfo& info)
	{
		return Ref<ImGuiMuleContext>(new ImGuiMuleContext(info));
	}
	void ImGuiMuleContext::AddEvent(Ref<WindowEvent> event)
	{
		switch (event->Type)
		{
		case Mule::WindowEventType::MouseMove:
			ImGui::GetIO().AddMousePosEvent(event->Mouse.MouseX, event->Mouse.MouseY);
			break;
		case Mule::WindowEventType::MouseButton:
			ImGui::GetIO().AddMouseButtonEvent((int)event->Mouse.Button, event->Mouse.Pressed);
			break;
		case Mule::WindowEventType::WindowResize:
			mDevice->ResizeSwapChain(event->Window.Width, event->Window.Height);
			mWidth = event->Window.Width;
			mHeight = event->Window.Height;
			break;
		default:
			break;
		}
	}

	void ImGuiMuleContext::BeginFrame(Ref<FrameBuffer> fb, int attachmentIndex)
	{
		const glm::vec4 clearColor = { 0, 0, 0, 0 };
		ImGui::SetCurrentContext(mCtx);

		if (fb == nullptr)
		{
			Diligent::ITextureView* views = mDevice->GetSwapChain()->GetCurrentBackBufferRTV();
			mContext.GetContext()->SetRenderTargets(
				1,
				&views,
				mDevice->GetSwapChain()->GetDepthBufferDSV(),
				Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
			mContext.GetContext()->ClearRenderTarget(views, &clearColor[0], Diligent::RESOURCE_STATE_TRANSITION_MODE_NONE);
		}
		mImGuiImpl->NewFrame(mWidth, mHeight, Diligent::SURFACE_TRANSFORM::SURFACE_TRANSFORM_IDENTITY);
	}

	void ImGuiMuleContext::Render()
	{
		ImGui::Render();
		mImGuiImpl->Render(mContext.GetContext());
	}

	ImGuiMuleContext::ImGuiMuleContext(const ImGuiMuleCreateInfo& info)
		:
		mDevice(info.Device),
		mContext(info.Context),
		mWidth(info.InitialWidth),
		mHeight(info.InitialHeight)
	{ 
		mCtx = ImGui::CreateContext();
		ImGui::StyleColorsDark();

		ImGui::GetIO().DisplaySize = { (float)mWidth, (float)mHeight};

		Diligent::ImGuiDiligentCreateInfo imguiInfo;

		auto diligentDevice = mDevice->GetRenderDevice();

		imguiInfo.pDevice = diligentDevice.Get();
		if (info.FrameBuffer)
		{
			imguiInfo.BackBufferFmt = info.FrameBuffer->AttachmentViews()[info.AttachmentIndex]->GetDesc().Format;
			imguiInfo.DepthBufferFmt = info.FrameBuffer->DepthAttachmentView()->GetDesc().Format;
		}
		else
		{
			imguiInfo.BackBufferFmt = mDevice->GetSwapChain()->GetCurrentBackBufferRTV()->GetDesc().Format;
			imguiInfo.DepthBufferFmt = mDevice->GetSwapChain()->GetDepthBufferDSV()->GetDesc().Format;
		}

		HWND hwnd = info.Window->GetWin32Window();
		mImGuiImpl = Diligent::ImGuiImplWin32::Create(imguiInfo, hwnd);
	}
}