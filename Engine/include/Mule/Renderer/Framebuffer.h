#pragma once

#include "Mule/Ref.h"

#include "Texture.h"

#include <bgfx/bgfx.h>

#include <vector>

namespace Mule
{
	struct FrameBufferInfo
	{
		FrameBufferInfo() = default;
		FrameBufferInfo(std::initializer_list<TextureFormat> formats, int width, int height)
			:
			Attachments(formats),
			Width(width),
			Height(height)
		{ }
		FrameBufferInfo(const FrameBufferInfo&) = default;

		std::vector<TextureFormat> Attachments;
		int Width;
		int Height;
	};

	class FrameBuffer
	{
	public:
		FrameBuffer() = default;
		FrameBuffer(const FrameBuffer&) = default;
		~FrameBuffer();

		static Ref<FrameBuffer> Create(const FrameBufferInfo& info);

		void Resize(int width, int height);
		void SetActive();
		void Render();
		static void SetMainViewport();

		void* GetAttachment(int index);

	private:
		FrameBufferInfo mInfo;
		bgfx::FrameBufferHandle mHandle;
		std::vector<bgfx::TextureHandle> mFramebufferAttachments;

		void CreateInternal(const FrameBufferInfo& info);
		void DestroyInternal();
	};
}