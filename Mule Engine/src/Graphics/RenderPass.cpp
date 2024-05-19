#include "Graphics/RenderPass.h"
#include "Graphics/FrameBuffer.h"

namespace Mule
{
	Ref<RenderPass> RenderPass::Create(WeakRef<GraphicsDevice> device, const RenderPassDescription& desc)
	{
		return Ref<RenderPass>(new RenderPass(device, desc));
	}
	
	Diligent::IRenderPass* RenderPass::GetRenderPass()
	{
		return mRenderPass.RawPtr();
	}

	void RenderPass::Begin(WeakRef<FrameBuffer> framebuffer)
	{
		mContext.GetContext()->SetRenderTargets(framebuffer->AttachmentViews().size(), framebuffer->AttachmentViews().data(), framebuffer->DepthAttachmentView(), Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

		Diligent::BeginRenderPassAttribs attribs{};

		std::vector<Diligent::OptimizedClearValue> clearValues = framebuffer->ClearValues();

		attribs.ClearValueCount = clearValues.size();
		attribs.pClearValues = clearValues.data();
		attribs.pRenderPass = mRenderPass;
		attribs.pFramebuffer = framebuffer->GetFrameBuffer();
		attribs.StateTransitionMode = Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION;
		mContext.GetContext()->BeginRenderPass(attribs);

		Diligent::Viewport viewport;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.MinDepth = 0;
		viewport.MaxDepth = 1;
		viewport.Width = framebuffer->GetWidth();
		viewport.Height = framebuffer->GetHeight();
		mContext.GetContext()->SetViewports(1, &viewport, framebuffer->GetWidth(), framebuffer->GetHeight());

		Diligent::Rect rects{};
		rects.left = 0.f;
		rects.right = 1.f;
		rects.top = 0.f;
		rects.bottom = 1.f;

		mContext.GetContext()->SetScissorRects(1, &rects, framebuffer->GetWidth(), framebuffer->GetHeight());
	}

	void RenderPass::NextPass()
	{
		mContext.GetContext()->NextSubpass();
	}

	void RenderPass::End()
	{
		mContext.GetContext()->EndRenderPass();
	}

	RenderPass::~RenderPass()
	{
	}
	
	RenderPass::RenderPass(WeakRef<GraphicsDevice> device, const RenderPassDescription& desc)
		:
		mContext(desc.Context)
	{
		auto diligentDevice = device->GetRenderDevice();

		std::vector<Diligent::RenderPassAttachmentDesc> attachments;
		for (int i = 0; i < desc.Attachments.size(); i++)
		{
			const RenderPassAttachment& renderPassAttachmentDesc = desc.Attachments[i];
			Diligent::RenderPassAttachmentDesc attachmentDesc{};
			attachmentDesc.Format = static_cast<Diligent::TEXTURE_FORMAT>(renderPassAttachmentDesc.Format);
			attachmentDesc.LoadOp = Diligent::ATTACHMENT_LOAD_OP_LOAD;
			attachmentDesc.StoreOp = Diligent::ATTACHMENT_STORE_OP_STORE;
			attachmentDesc.InitialState = Diligent::RESOURCE_STATE::RESOURCE_STATE_RENDER_TARGET;
			attachmentDesc.FinalState = Diligent::RESOURCE_STATE::RESOURCE_STATE_SHADER_RESOURCE;
			attachmentDesc.StencilLoadOp = Diligent::ATTACHMENT_LOAD_OP_DISCARD;
			attachmentDesc.StencilStoreOp = Diligent::ATTACHMENT_STORE_OP_DISCARD;
			//attachmentDesc.SampleCount = static_cast<uint32_t>(renderPassAttachmentDesc.SampleCount);

			attachments.push_back(attachmentDesc);
		}


		// Depth
		{
			const RenderPassAttachment& renderPassAttachmentDesc = desc.DepthAttachment;
			Diligent::RenderPassAttachmentDesc attachmentDesc{};
			attachmentDesc.Format = static_cast<Diligent::TEXTURE_FORMAT>(renderPassAttachmentDesc.Format);
			attachmentDesc.LoadOp = Diligent::ATTACHMENT_LOAD_OP_LOAD;
			attachmentDesc.StoreOp = Diligent::ATTACHMENT_STORE_OP_STORE;
			attachmentDesc.InitialState = Diligent::RESOURCE_STATE::RESOURCE_STATE_DEPTH_WRITE;
			attachmentDesc.FinalState = Diligent::RESOURCE_STATE::RESOURCE_STATE_SHADER_RESOURCE;
			attachmentDesc.StencilLoadOp = Diligent::ATTACHMENT_LOAD_OP_DISCARD;
			attachmentDesc.StencilStoreOp = Diligent::ATTACHMENT_STORE_OP_DISCARD;

			attachments.push_back(attachmentDesc);
		}



		Diligent::AttachmentReference depthAttachment;
		depthAttachment.AttachmentIndex = desc.Attachments.size(); // depth attachment will always be the last index
		depthAttachment.State = Diligent::RESOURCE_STATE_DEPTH_WRITE;
		
		std::vector<Diligent::SubpassDesc> subPasses;
		std::vector<Diligent::AttachmentReference*> refs;
		for (int i = 0; i < desc.SubPasses.size(); i++)
		{
			const RenderPassSubPassDescription& subPass = desc.SubPasses[i];

			Diligent::SubpassDesc subpassDesc{};

			Diligent::AttachmentReference* inputAttachmentRefs = new Diligent::AttachmentReference[subPass.InputAttachments.size()];
			for (int j = 0; j < subPass.InputAttachments.size(); j++)
			{
				Diligent::AttachmentReference ref;
				ref.AttachmentIndex = subPass.OutputAttachments[i];
				ref.State = Diligent::RESOURCE_STATE::RESOURCE_STATE_COMMON;
				inputAttachmentRefs[j] = ref;
			}
			subpassDesc.InputAttachmentCount = subPass.InputAttachments.size();
			subpassDesc.pInputAttachments = inputAttachmentRefs;
			refs.push_back(inputAttachmentRefs);
			
			if (subPass.RenderToDepth)
			{
				subpassDesc.pDepthStencilAttachment = &depthAttachment;
			}

			bool msaaInPass = false;

			Diligent::AttachmentReference* outputAttachmentRefs = new Diligent::AttachmentReference[subPass.OutputAttachments.size()];
			for (int j = 0; j < subPass.OutputAttachments.size(); j++)
			{
				Diligent::AttachmentReference ref;
				ref.AttachmentIndex = subPass.OutputAttachments[j];
				ref.State = Diligent::RESOURCE_STATE::RESOURCE_STATE_COMMON;
				outputAttachmentRefs[j] = ref;

				if (desc.Attachments[ref.AttachmentIndex].SampleCount != Samples::SampleCount_1)
					msaaInPass = true;
			}
			subpassDesc.RenderTargetAttachmentCount = subPass.OutputAttachments.size();
			subpassDesc.pRenderTargetAttachments = outputAttachmentRefs;
			refs.push_back(outputAttachmentRefs);

			subpassDesc.PreserveAttachmentCount;
			subpassDesc.pPreserveAttachments;

			if(msaaInPass)
				subpassDesc.pResolveAttachments = outputAttachmentRefs;

			subPasses.push_back(subpassDesc);
		}

		Diligent::RenderPassDesc renderPassDesc{};
		renderPassDesc.Name = desc.Name.c_str();
		renderPassDesc.AttachmentCount = attachments.size();
		renderPassDesc.pAttachments = attachments.data();
		renderPassDesc.SubpassCount = subPasses.size();
		renderPassDesc.pSubpasses = subPasses.data();
		renderPassDesc.DependencyCount = 0;
		renderPassDesc.pDependencies = nullptr;

		diligentDevice->CreateRenderPass(renderPassDesc, &mRenderPass);

		for (auto* ref : refs)
		{
			delete[] ref;
		}
	}
}