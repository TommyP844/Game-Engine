#pragma once

#include "GraphicsDevice.h"
#include "GraphicsTypes.h"
#include "RenderContext.h"

#include "DiligentCore/Graphics/GraphicsEngine/interface/RenderPass.h"

namespace Mule
{

	struct RenderPassSubPassDescription
	{
		std::vector<uint32_t> InputAttachments;
		std::vector<uint32_t> OutputAttachments;
		bool RenderToDepth;
	};

	struct RenderPassAttachment
	{
		TextureFormat Format;
		Samples SampleCount;
	};

	struct RenderPassDescription
	{
		std::string Name;
		std::vector<RenderPassAttachment> Attachments;
		RenderPassAttachment DepthAttachment;
		std::vector<RenderPassSubPassDescription> SubPasses;
		RenderContext Context;
	};

	class FrameBuffer;

	class RenderPass
	{
	public:
		static Ref<RenderPass> Create(WeakRef<GraphicsDevice> device, const RenderPassDescription& desc);

		Diligent::IRenderPass* GetRenderPass();

		void Begin(WeakRef<FrameBuffer> framebuffer);
		void NextPass();
		void End();

		~RenderPass();
	private:
		RenderPass(WeakRef<GraphicsDevice> device, const RenderPassDescription& desc);

		RenderContext mContext;
		Diligent::RefCntAutoPtr<Diligent::IRenderPass> mRenderPass;
	};
}
