#pragma once

#include "Ref.h"
#include "imgui.h"
#include "WindowEvent.h"
#include "GraphicsDevice.h"
#include "FrameBuffer.h"

#include "DiligentTools/Imgui/interface/ImGuiImplDiligent.hpp"

namespace Mule
{
	struct ImGuiMuleCreateInfo
	{
		WeakRef<GraphicsDevice> Device;
		RenderContext Context;
		WeakRef<Window> Window;

		// If this is null sap chain will be used
		WeakRef<FrameBuffer> FrameBuffer;
		int AttachmentIndex;

		int InitialWidth;
		int InitialHeight;

	};

	class ImGuiMuleContext
	{
	public:
		static Ref<ImGuiMuleContext> Create(const ImGuiMuleCreateInfo& info);

		void AddEvent(Ref<WindowEvent> event);
		void BeginFrame(Ref<FrameBuffer> fb = nullptr, int attachmentIndex = 0);
		void Render();
	private:
		ImGuiMuleContext(const ImGuiMuleCreateInfo& info);

		int mWidth, mHeight;

		ImGuiContext* mCtx;
		RenderContext mContext;
		WeakRef<GraphicsDevice> mDevice;
		std::unique_ptr<Diligent::ImGuiImplDiligent> mImGuiImpl;
	};
}