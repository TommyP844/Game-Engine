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
		return mAttachmentTextures[index]->GetImGuiID();
	}

	FrameBuffer::~FrameBuffer()
	{
	}

	FrameBuffer::FrameBuffer(WeakRef<GraphicsDevice> device, const FrameBufferDescription& desc)
	{
		Init(device, desc);
	}

	FrameBuffer::FrameBuffer()
	{
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
			mAttachmentViews.push_back(device->GetSwapChain()->GetCurrentBackBufferRTV());
			mDepthView = device->GetSwapChain()->GetDepthBufferDSV();

			renderTargetViews = { mAttachmentViews[0], mDepthView};
		}
		else
		{
			for (int i = 0; i < desc.Attachments.size(); i++)
			{
				TextureDescription textureDesc;

				textureDesc.Width = desc.Width;
				textureDesc.Height = desc.Height;
				textureDesc.Depth = 1;
				textureDesc.Layers = 1;
				textureDesc.SampleCount = desc.SampleCount;
				textureDesc.Type = TextureType::Type_2D;
				textureDesc.Format = desc.Attachments[i].Format;
				textureDesc.Name = desc.Name + " - Attachment " + std::to_string(i);
				textureDesc.Flags = (TextureFlags)(TextureFlags::RenderTarget | desc.Attachments[i].Flags);

				auto attachment = Texture::Create(device, desc.Context, textureDesc);
				mAttachmentTextures.push_back(attachment);
				mAttachmentViews.push_back(attachment->GetRenderTargetView());

				bool isDepth = desc.Attachments[i].Format == TextureFormat::Depth16U
					|| desc.Attachments[i].Format == TextureFormat::Depth24Stencil8
					|| desc.Attachments[i].Format == TextureFormat::Depth32F;

				Diligent::OptimizedClearValue clearValue;
				clearValue.Format = static_cast<Diligent::TEXTURE_FORMAT>(desc.Attachments[i].Format);
				if (isDepth)
				{
					renderTargetViews.push_back(attachment->GetDepthView());
					clearValue.Color[0] = desc.Attachments[i].ClearColor[0];
					clearValue.Color[1] = desc.Attachments[i].ClearColor[1];
					clearValue.Color[2] = desc.Attachments[i].ClearColor[2];
					clearValue.Color[3] = desc.Attachments[i].ClearColor[3];
					mDepthView = attachment->GetDepthView();
				}
				else
				{
					clearValue.DepthStencil.Depth = desc.Attachments[i].ClearColor[0];
					renderTargetViews.push_back(attachment->GetRenderTargetView());
				}
				mTextureClearValues.push_back(clearValue);
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