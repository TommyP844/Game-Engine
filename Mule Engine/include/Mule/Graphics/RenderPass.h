#pragma once

#include "GraphicsDevice.h"
#include "GraphicsTypes.h"
#include "RenderContext.h"
#include "Asset/Asset.h"

#include "DiligentCore/Graphics/GraphicsEngine/interface/RenderPass.h"

namespace Mule
{

	struct RenderPassSubPassDescription
	{
		RenderPassSubPassDescription(
			const std::initializer_list<uint32_t>& inputAttachments, 
			const std::initializer_list<uint32_t>& outputAttachments, 
			bool renderToDepth)
			:
			InputAttachments(inputAttachments),
			OutputAttachments(outputAttachments),
			RenderToDepth(renderToDepth)
		{}
		std::vector<uint32_t> InputAttachments;
		std::vector<uint32_t> OutputAttachments;
		bool RenderToDepth;
	};

	struct RenderPassAttachment
	{
		RenderPassAttachment() = default;
		RenderPassAttachment(TextureFormat format, uint32_t samples)
			:
			Format(format),
			SampleCount(samples)
		{}

		TextureFormat Format;
		uint32_t SampleCount;
	};

	struct RenderPassDescription
	{
		RenderPassDescription() = default;
		std::string Name;
		std::vector<RenderPassAttachment> Attachments;
		RenderPassAttachment DepthAttachment;
		std::vector<RenderPassSubPassDescription> SubPasses;
		RenderContext Context;
	};

	class FrameBuffer;

	class RenderPass : public Asset
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
