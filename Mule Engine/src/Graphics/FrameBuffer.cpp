#include "Graphics/FrameBuffer.h"

namespace Mule
{
	Ref<FrameBuffer> FrameBuffer::Create(WeakRef<GraphicsDevice> device, const FrameBufferDescription& desc)
	{
		return Ref<FrameBuffer>(new FrameBuffer(device, desc));
	}

	void FrameBuffer::Resize(int width, int height)
	{
		mAttachmentViews.clear();
		mAttachmentTextures.clear();
		mTextureClearValues.clear();
		mFrameBufferDesc.Width = width;
		mFrameBufferDesc.Height = height;
		Init(mDevice, mFrameBufferDesc);
	}

	ImTextureID FrameBuffer::GetAttachmentViewImGui(int index)
	{
		return static_cast<ImTextureID>(mSRAttachmentViews[index]);
	}

	FrameBuffer::~FrameBuffer()
	{
	}

	FrameBuffer::FrameBuffer(WeakRef<GraphicsDevice> device, const FrameBufferDescription& desc)
	{
		Init(device, desc);
	}

	void FrameBuffer::Init(WeakRef<GraphicsDevice> device, const FrameBufferDescription& desc)
	{
		mWidth = desc.Width;
		mHeight = desc.Height;
		mTextureClearValues.clear();
		mDevice = device;
		mFrameBufferDesc = desc;
		mContext = desc.Context;
		auto diligentDevice = device->GetRenderDevice();

		std::vector<Diligent::ITextureView*> renderTargetViews;

		if (desc.BuildForSwapChain)
		{
			Diligent::ITextureView* view = device->GetSwapChain()->GetCurrentBackBufferRTV();
			mAttachmentViews.push_back(view);
			mDepthAttachmentView = device->GetSwapChain()->GetDepthBufferDSV();
			 
			renderTargetViews = { mAttachmentViews[0], mDepthAttachmentView };
		}
		else
		{
			for (int i = 0; i < desc.Attachments.size(); i++)
			{
				auto& attachmentDesc = desc.Attachments[i];

				Diligent::TextureDesc textureDesc{};
				textureDesc.ArraySize = 1;
				textureDesc.Width = desc.Width;
				textureDesc.Height = desc.Height;
				textureDesc.Depth = 1;
				textureDesc.Format = static_cast<Diligent::TEXTURE_FORMAT>(attachmentDesc.Format);
				textureDesc.SampleCount = attachmentDesc.Samples;
				textureDesc.Type = Diligent::RESOURCE_DIM_TEX_2D;
				textureDesc.BindFlags = Diligent::BIND_RENDER_TARGET | Diligent::BIND_SHADER_RESOURCE;

				Diligent::RefCntAutoPtr<Diligent::ITexture> texture;
				diligentDevice->CreateTexture(textureDesc, nullptr, &texture);
				mAttachmentTextures.push_back(texture);

				Diligent::TextureViewDesc viewDesc{};
				viewDesc.ViewType = Diligent::TEXTURE_VIEW_RENDER_TARGET;
				viewDesc.Flags = Diligent::TEXTURE_VIEW_FLAG_NONE;
				viewDesc.TextureDim = Diligent::RESOURCE_DIM_TEX_2D;

				Diligent::RefCntAutoPtr<Diligent::ITextureView> textureView;
				texture->CreateView(viewDesc, &textureView);
				mAttachmentViews.push_back(textureView);

				viewDesc.ViewType = Diligent::TEXTURE_VIEW_SHADER_RESOURCE;
				Diligent::RefCntAutoPtr<Diligent::ITextureView> srTextureView;
				texture->CreateView(viewDesc, &srTextureView);
				mSRAttachmentViews.push_back(srTextureView);
			}

			for (auto& view : mAttachmentViews)
			{
				renderTargetViews.push_back(view);
			}

			// Depth
			if (desc.HasDepth)
			{
				auto& attachmentDesc = desc.DepthAttachment;

				Diligent::TextureDesc textureDesc{};
				textureDesc.ArraySize = 1;
				textureDesc.Width = desc.Width;
				textureDesc.Height = desc.Height;
				textureDesc.Depth = 1;
				textureDesc.Format = static_cast<Diligent::TEXTURE_FORMAT>(attachmentDesc.Format);
				textureDesc.SampleCount = attachmentDesc.Samples;
				textureDesc.Type = Diligent::RESOURCE_DIM_TEX_2D;
				textureDesc.BindFlags = Diligent::BIND_DEPTH_STENCIL | Diligent::BIND_SHADER_RESOURCE;

				diligentDevice->CreateTexture(textureDesc, nullptr, &mDepthAttachmentTexture);

				Diligent::TextureViewDesc viewDesc{};
				viewDesc.ViewType = Diligent::TEXTURE_VIEW_READ_ONLY_DEPTH_STENCIL;
				viewDesc.Flags = Diligent::TEXTURE_VIEW_FLAG_NONE;
				viewDesc.TextureDim = Diligent::RESOURCE_DIM_TEX_2D;

				mDepthAttachmentTexture->CreateView(viewDesc, &mDepthAttachmentView);

				viewDesc.ViewType = Diligent::TEXTURE_VIEW_SHADER_RESOURCE;
				Diligent::RefCntAutoPtr<Diligent::ITextureView> srTextureView;
				mDepthAttachmentTexture->CreateView(viewDesc, &srTextureView);
				mSRAttachmentViews.push_back(srTextureView);

				//renderTargetViews.push_back(mDepthAttachmentView);
			}
		}

		Diligent::FramebufferDesc frameBufferDesc;

		frameBufferDesc.Name = desc.Name.c_str();
		frameBufferDesc.Width = desc.Width;
		frameBufferDesc.Height = desc.Height;
		frameBufferDesc.NumArraySlices = 1;
		frameBufferDesc.AttachmentCount = renderTargetViews.size();
		frameBufferDesc.ppAttachments = renderTargetViews.data();
		frameBufferDesc.pRenderPass = desc.RenderPass->GetRenderPass();

		Diligent::RefCntAutoPtr<Diligent::IFramebuffer> framebuffer;
		diligentDevice->CreateFramebuffer(frameBufferDesc, &framebuffer);
		mFrameBuffer = framebuffer;
	}
	
}