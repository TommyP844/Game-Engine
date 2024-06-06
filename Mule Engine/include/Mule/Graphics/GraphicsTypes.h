#pragma once

#include "DiligentCore/Graphics/GraphicsEngine/interface/GraphicsTypes.h"


namespace Mule
{
	enum TextureFormat
	{
		RGBA8 = Diligent::TEXTURE_FORMAT::TEX_FORMAT_RGBA8_UNORM,
		RGBA8_SRGB = Diligent::TEXTURE_FORMAT::TEX_FORMAT_RGBA8_UNORM_SRGB,

		// Depth Formats
		Depth24Stencil8 = Diligent::TEXTURE_FORMAT::TEX_FORMAT_D24_UNORM_S8_UINT,
		Depth16U = Diligent::TEXTURE_FORMAT::TEX_FORMAT_D16_UNORM,
		Depth32F = Diligent::TEXTURE_FORMAT::TEX_FORMAT_D32_FLOAT,
	};

	enum class Samples : uint32_t
	{
		SampleCount_1 = 1,
		SampleCount_2 = 2,
		SampleCount_4 = 4,
		SampleCount_8 = 8,
		SampleCount_16 = 16,
		SampleCount_32 = 32,
		SampleCount_64 = 64
	};

	enum AttachmentLoadOp
	{
		Discard = Diligent::ATTACHMENT_LOAD_OP_DISCARD,
		Load = Diligent::ATTACHMENT_LOAD_OP_LOAD,
	};

	enum BufferAccess
	{
		Read = Diligent::CPU_ACCESS_READ,
		Write = Diligent::CPU_ACCESS_WRITE,
		ReadWrite = Diligent::CPU_ACCESS_READ | Diligent::CPU_ACCESS_WRITE
	};

	static uint32_t TextureFormatSize(TextureFormat format)
	{
		switch (format)
		{
		case Mule::RGBA8:
		case Mule::RGBA8_SRGB:
		case Mule::Depth24Stencil8:
			return 4;
		case Mule::Depth16U:
			return 8;
		case Mule::Depth32F:
			return 16;
		}

		return 0;
	}
}