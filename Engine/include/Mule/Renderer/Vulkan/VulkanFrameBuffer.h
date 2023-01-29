#pragma once

#include "Mule/Ref.h"

#include <vulkan/vulkan.hpp>

#include <initializer_list>

namespace Mule
{
	enum class AttachmentFormat
	{

	};

	struct FrameBufferAttachment
	{
		
	};

	class FrameBuffer
	{
	public:
		FrameBuffer(const std::initializer_list<FrameBufferAttachment>& list);
		~FrameBuffer();

		Ref<FrameBuffer> Create(const std::initializer_list<FrameBufferAttachment>& list);
	
	private:
		VkFramebuffer mFrameBuffer;
		
	};
}