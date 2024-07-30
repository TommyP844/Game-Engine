#include "Graphics/ImGuiMuleContext.h"

#include "imgui.h"

#include <glm.hpp>

#include <filesystem>

namespace fs = std::filesystem;

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
		case Mule::WindowEventType::Char:
			ImGui::GetIO().AddInputCharacter((unsigned int)event->Keyboard.Char);
			break;
		case Mule::WindowEventType::Key:
			ImGui::GetIO().AddKeyEvent(mKeyMapping[event->Keyboard.Key], event->Keyboard.Pressed);
			break;
        case Mule::WindowEventType::Scroll:
            ImGui::GetIO().AddMouseWheelEvent(event->Mouse.ScrollXOffset, event->Mouse.ScrollYOffset);
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
		//ImGui::NewFrame();
		mImGuiImpl->NewFrame(mWidth, mHeight, Diligent::SURFACE_TRANSFORM::SURFACE_TRANSFORM_IDENTITY);
	}

	void ImGuiMuleContext::Render()
	{
		mImGuiImpl->EndFrame();
		mImGuiImpl->Render(mContext.GetContext());
	}

	ImGuiMuleContext::~ImGuiMuleContext()
	{
		ImGui::SetCurrentContext(mCtx);
		ImGuiIO& io = ImGui::GetIO();
		IM_FREE(io.BackendPlatformUserData);
		IM_FREE(io.BackendRendererUserData);
		io.BackendPlatformUserData = NULL;
		io.BackendRendererUserData = NULL;
		mImGuiImpl.release();
		ImGui::SaveIniSettingsToDisk("imgui.ini");
		ImGui::DestroyContext(mCtx);
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

		ImGuiIO& io = ImGui::GetIO();

		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		
		if (fs::exists("imgui.ini"))
		{
			ImGui::LoadIniSettingsFromDisk("imgui.ini");
		}
		ImGui::GetIO().DisplaySize = { (float)mWidth, (float)mHeight};

		Diligent::ImGuiDiligentCreateInfo imguiInfo;

        ImFont* font = io.Fonts->AddFontFromFileTTF("C:/Dev/Mule-Engine/Resources/Fonts/Roboto/Roboto-Black.ttf", 20.f);
        

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

		mKeyMapping = {
            { Key::Key_Space, ImGuiKey_Space },
            { Key::Key_Apostrophe, ImGuiKey_Apostrophe },
            { Key::Key_Comma, ImGuiKey_Comma },
            { Key::Key_Minus, ImGuiKey_Minus },
            { Key::Key_Period, ImGuiKey_Period },
            { Key::Key_Slash, ImGuiKey_Slash },
            { Key::Key_0, ImGuiKey_0 },
            { Key::Key_1, ImGuiKey_1 },
            { Key::Key_2, ImGuiKey_2 },
            { Key::Key_3, ImGuiKey_3 },
            { Key::Key_4, ImGuiKey_4 },
            { Key::Key_5, ImGuiKey_5 },
            { Key::Key_6, ImGuiKey_6 },
            { Key::Key_7, ImGuiKey_7 },
            { Key::Key_8, ImGuiKey_8 },
            { Key::Key_9, ImGuiKey_9 },
            { Key::Key_Semicolon, ImGuiKey_Semicolon },
            { Key::Key_Equal, ImGuiKey_Equal },
            { Key::Key_A, ImGuiKey_A },
            { Key::Key_B, ImGuiKey_B },
            { Key::Key_C, ImGuiKey_C },
            { Key::Key_D, ImGuiKey_D },
            { Key::Key_E, ImGuiKey_E },
            { Key::Key_F, ImGuiKey_F },
            { Key::Key_G, ImGuiKey_G },
            { Key::Key_H, ImGuiKey_H },
            { Key::Key_I, ImGuiKey_I },
            { Key::Key_J, ImGuiKey_J },
            { Key::Key_K, ImGuiKey_K },
            { Key::Key_L, ImGuiKey_L },
            { Key::Key_M, ImGuiKey_M },
            { Key::Key_N, ImGuiKey_N },
            { Key::Key_O, ImGuiKey_O },
            { Key::Key_P, ImGuiKey_P },
            { Key::Key_Q, ImGuiKey_Q },
            { Key::Key_R, ImGuiKey_R },
            { Key::Key_S, ImGuiKey_S },
            { Key::Key_T, ImGuiKey_T },
            { Key::Key_U, ImGuiKey_U },
            { Key::Key_V, ImGuiKey_V },
            { Key::Key_W, ImGuiKey_W },
            { Key::Key_X, ImGuiKey_X },
            { Key::Key_Y, ImGuiKey_Y },
            { Key::Key_Z, ImGuiKey_Z },
            { Key::Key_LeftBracket, ImGuiKey_LeftBracket },
            { Key::Key_Backslash, ImGuiKey_Backslash },
            { Key::Key_RightBracket, ImGuiKey_RightBracket },
            { Key::Key_GraveAccent, ImGuiKey_GraveAccent },
            { Key::Key_World1, ImGuiKey_None },
            { Key::Key_World2, ImGuiKey_None },
            { Key::Key_Escape, ImGuiKey_Escape },
            { Key::Key_Enter, ImGuiKey_Enter },
            { Key::Key_Tab, ImGuiKey_Tab },
            { Key::Key_Backspace, ImGuiKey_Backspace },
            { Key::Key_Insert, ImGuiKey_Insert },
            { Key::Key_Delete, ImGuiKey_Delete },
            { Key::Key_Right, ImGuiKey_RightArrow },
            { Key::Key_Left, ImGuiKey_LeftArrow },
            { Key::Key_Down, ImGuiKey_DownArrow },
            { Key::Key_Up, ImGuiKey_UpArrow },
            { Key::Key_PageUp, ImGuiKey_PageUp },
            { Key::Key_PageDown, ImGuiKey_PageDown },
            { Key::Key_Home, ImGuiKey_Home },
            { Key::Key_End, ImGuiKey_End },
            { Key::Key_CapsLock, ImGuiKey_CapsLock },
            { Key::Key_ScrollLock, ImGuiKey_ScrollLock },
            { Key::Key_NumLock, ImGuiKey_NumLock },
            { Key::Key_PrintScreen, ImGuiKey_PrintScreen },
            { Key::Key_Pause, ImGuiKey_Pause },
            { Key::Key_F1, ImGuiKey_F1 },
            { Key::Key_F2, ImGuiKey_F2 },
            { Key::Key_F3, ImGuiKey_F3 },
            { Key::Key_F4, ImGuiKey_F4 },
            { Key::Key_F5, ImGuiKey_F5 },
            { Key::Key_F6, ImGuiKey_F6 },
            { Key::Key_F7, ImGuiKey_F7 },
            { Key::Key_F8, ImGuiKey_F8 },
            { Key::Key_F9, ImGuiKey_F9 },
            { Key::Key_F10, ImGuiKey_F10 },
            { Key::Key_F11, ImGuiKey_F11 },
            { Key::Key_F12, ImGuiKey_F12 },
            { Key::Key_F13, ImGuiKey_F13 },
            { Key::Key_F14, ImGuiKey_F14 },
            { Key::Key_F15, ImGuiKey_F15 },
            { Key::Key_F16, ImGuiKey_F16 },
            { Key::Key_F17, ImGuiKey_F17 },
            { Key::Key_F18, ImGuiKey_F18 },
            { Key::Key_F19, ImGuiKey_F19 },
            { Key::Key_F20, ImGuiKey_F20 },
            { Key::Key_F21, ImGuiKey_F21 },
            { Key::Key_F22, ImGuiKey_F22 },
            { Key::Key_F23, ImGuiKey_F23 },
            { Key::Key_F24, ImGuiKey_F24 },
            { Key::Key_F25, ImGuiKey_None },
            { Key::Key_KP0, ImGuiKey_Keypad0 },
            { Key::Key_KP1, ImGuiKey_Keypad1 },
            { Key::Key_KP2, ImGuiKey_Keypad2 },
            { Key::Key_KP3, ImGuiKey_Keypad3 },
            { Key::Key_KP4, ImGuiKey_Keypad4 },
            { Key::Key_KP5, ImGuiKey_Keypad5 },
            { Key::Key_KP6, ImGuiKey_Keypad6 },
            { Key::Key_KP7, ImGuiKey_Keypad7 },
            { Key::Key_KP8, ImGuiKey_Keypad8 },
            { Key::Key_KP9, ImGuiKey_Keypad9 },
            { Key::Key_KPDecimal, ImGuiKey_KeypadDecimal },
            { Key::Key_KPDivide, ImGuiKey_KeypadDivide },
            { Key::Key_KPMultiply, ImGuiKey_KeypadMultiply },
            { Key::Key_KPSubtract, ImGuiKey_KeypadSubtract },
            { Key::Key_KPAdd, ImGuiKey_KeypadAdd },
            { Key::Key_KPEnter, ImGuiKey_KeypadEnter },
            { Key::Key_KPEqual, ImGuiKey_KeypadEqual },
            { Key::Key_LeftShift, ImGuiKey_LeftShift },
            { Key::Key_LeftControl, ImGuiKey_LeftCtrl },
            { Key::Key_LeftAlt, ImGuiKey_LeftAlt },
            { Key::Key_LeftSuper, ImGuiKey_LeftSuper },
            { Key::Key_RightShift, ImGuiKey_RightShift },
            { Key::Key_RightControl, ImGuiKey_RightCtrl },
            { Key::Key_RightAlt, ImGuiKey_RightAlt },
            { Key::Key_RightSuper, ImGuiKey_RightSuper },
            { Key::Key_Menu, ImGuiKey_Menu }
        };
	}
}