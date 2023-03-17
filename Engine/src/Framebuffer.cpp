#include "mulepch.h"
#include "Mule/Renderer/Framebuffer.h"
#include "Mule/bgfxImGui/imgui.h"

namespace Mule
{
	FrameBuffer::~FrameBuffer()
	{
		DestroyInternal();
	}

	Ref<FrameBuffer> FrameBuffer::Create(const FrameBufferInfo& info)
	{
		Ref<FrameBuffer> framebuffer = MakeRef<FrameBuffer>();
		framebuffer->mInfo = info;
		framebuffer->CreateInternal(info);
		return framebuffer;
	}

	void FrameBuffer::Resize(int width, int height)
	{
		DestroyInternal();

		mInfo.Width = width;
		mInfo.Height = height;

		CreateInternal(mInfo);
	}

	void FrameBuffer::SetActive()
	{
		bgfx::setViewFrameBuffer(0, mHandle);

		bgfx::touch(0);
		bgfx::setState(BGFX_STATE_DEFAULT);
		bgfx::setViewRect(0, 0, 0, mInfo.Width, mInfo.Height);
		bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0xAAAAFFFF, 1.f);
	}

	void FrameBuffer::Render()
	{
		bgfx::frame();
	}

	void FrameBuffer::SetMainViewport()
	{		
		bgfx::setViewFrameBuffer(0, BGFX_INVALID_HANDLE);
	}

	void* FrameBuffer::GetAttachment(int index)
	{
		auto handle = bgfx::getTexture(mHandle, index);
		return ImGui::toId(handle, 0, 0);;
	}

	void FrameBuffer::CreateInternal(const FrameBufferInfo& info)
	{
		for (const auto& attachment : info.Attachments)
		{
			bgfx::TextureHandle attachmentHandle = bgfx::createTexture2D(info.Width, info.Height, false, 1, (bgfx::TextureFormat::Enum)attachment, BGFX_TEXTURE_RT);
			
			mFramebufferAttachments.push_back(attachmentHandle);
		}

		bgfx::FrameBufferHandle handle = bgfx::createFrameBuffer(mFramebufferAttachments.size(), &mFramebufferAttachments[0]);
		
		mHandle = handle;
	}

	void FrameBuffer::DestroyInternal()
	{
		for (auto handle : mFramebufferAttachments)
		{
			bgfx::destroy(handle);
		}
		mFramebufferAttachments.clear();
		bgfx::destroy(mHandle);
	}
}