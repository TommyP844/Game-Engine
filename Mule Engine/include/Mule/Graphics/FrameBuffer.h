#pragma once

#include "GraphicsDevice.h"
#include "GraphicsTypes.h"
#include "Texture.h"
#include "RenderPass.h"
#include <glm.hpp>

namespace Mule
{
	struct FrameBufferAttachment
	{
		TextureFormat Format;
		TextureFlags Flags;
		//Depth uses x Component
		glm::vec4 ClearColor;
		uint32_t Samples = 1;
	};

	struct FrameBufferDescription
	{
		std::string Name;
		Ref<RenderPass> RenderPass;
		RenderContext Context;
		uint32_t Width;
		uint32_t Height;
		bool BuildForSwapChain;
		std::vector<FrameBufferAttachment> Attachments;
		FrameBufferAttachment DepthAttachment;
		bool HasDepth = false;
	};


	class FrameBuffer
	{
	public:
		static Ref<FrameBuffer> Create(WeakRef<GraphicsDevice> device, const FrameBufferDescription& desc);

		void Resize(int width, int height);

		uint32_t GetWidth() const { return mWidth; }
		uint32_t GetHeight() const { return mHeight; }

		Diligent::IFramebuffer* GetFrameBuffer() const { return mFrameBuffer; }
		std::vector<Diligent::OptimizedClearValue> ClearValues() const { return mTextureClearValues; }
		std::vector<Diligent::ITextureView*> AttachmentViews() const { return mAttachmentViews; }
		Diligent::ITextureView* GetAttachmentView(int index) const { return mAttachmentViews[index]; }
		ImTextureID GetAttachmentViewImGui(int index);
		Diligent::ITextureView* DepthAttachmentView() const { return mDepthAttachmentView; }

		uint32_t GetNumAttachments() const { return mAttachmentViews.size(); }

		~FrameBuffer();
	private:
		FrameBuffer(WeakRef<GraphicsDevice> device, const FrameBufferDescription& desc);
		void Init(WeakRef<GraphicsDevice> device, const FrameBufferDescription& desc);

		uint32_t mWidth;
		uint32_t mHeight;

		WeakRef<GraphicsDevice> mDevice;
		RenderContext mContext;

		FrameBufferDescription mFrameBufferDesc;

		Diligent::RefCntAutoPtr<Diligent::IFramebuffer> mFrameBuffer;
		std::vector<Diligent::ITextureView*> mAttachmentViews;
		std::vector<Diligent::ITextureView*> mSRAttachmentViews;
		std::vector<Diligent::ITexture*> mAttachmentTextures;
		Diligent::ITextureView* mDepthAttachmentView = nullptr;
		Diligent::ITexture* mDepthAttachmentTexture = nullptr;
		std::vector<Diligent::OptimizedClearValue> mTextureClearValues;
	};
}
