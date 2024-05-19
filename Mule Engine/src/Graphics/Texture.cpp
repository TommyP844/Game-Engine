#include "Graphics/Texture.h"
#include "Log.h"

#include "DiligentTools/Imgui/interface/ImGuiDiligentRenderer.hpp"

#include <math.h>

#ifdef max
#undef max
#endif

namespace Mule
{
	Ref<Texture> Texture::Create(WeakRef<GraphicsDevice> device, const TextureDescription& desc)
	{
		return Ref<Texture>(new Texture(device, desc));
	}

	Texture::Texture(WeakRef<GraphicsDevice> device, const TextureDescription& desc)
		:
		mWidth(desc.Width),
		mHeight(desc.Height),
		mDepth(desc.Depth),
		mLayers(desc.Layers),
		mMips(1),
		mFormat(desc.Format),
		mType(desc.Type),
		mSampleCount(desc.SampleCount),
		mName(desc.Name),
		mGraphicsDevice(device)
	{
		auto diligentDevice = device->GetRenderDevice();


		bool isDepth = (desc.Format & TextureFormat::Depth16U) == TextureFormat::Depth16U
			|| (desc.Format & TextureFormat::Depth24Stencil8) == TextureFormat::Depth24Stencil8
			|| (desc.Format & TextureFormat::Depth32F) == TextureFormat::Depth32F;
		
		if((desc.Flags & TextureFlags::GenerateMips) == TextureFlags::GenerateMips)
			mMips = std::log2(std::max(desc.Width, desc.Height));

		Diligent::CPU_ACCESS_FLAGS accessFlags = Diligent::CPU_ACCESS_NONE;

		if (desc.Flags & TextureFlags::CPUReadable)
			accessFlags |= Diligent::CPU_ACCESS_READ;
		if (desc.Flags & TextureFlags::CPUWriteable)
			accessFlags |= Diligent::CPU_ACCESS_WRITE;

		Diligent::BIND_FLAGS bindFlags = Diligent::BIND_FLAGS::BIND_NONE;
		if (desc.Flags & TextureFlags::RenderTarget)
			bindFlags |= Diligent::BIND_FLAGS::BIND_RENDER_TARGET;

		if (desc.Flags & TextureFlags::RenderTarget)
			bindFlags |= Diligent::BIND_FLAGS::BIND_SHADER_RESOURCE;

		if (isDepth)
			bindFlags |= Diligent::BIND_FLAGS::BIND_DEPTH_STENCIL;


		Diligent::TextureDesc textureDesc;
		textureDesc.Name = desc.Name.c_str();
		textureDesc.Width = desc.Width;
		textureDesc.Height = desc.Height;
		textureDesc.Depth = desc.Depth;
		textureDesc.CPUAccessFlags = accessFlags;
		textureDesc.Type = static_cast<Diligent::RESOURCE_DIMENSION>(desc.Type);
		textureDesc.MipLevels = mMips;
		textureDesc.SampleCount = static_cast<uint32_t>(desc.SampleCount);
		textureDesc.ArraySize = desc.Layers;
		textureDesc.Format = static_cast<Diligent::TEXTURE_FORMAT>(desc.Format);
		textureDesc.BindFlags = bindFlags;

		diligentDevice->CreateTexture(textureDesc, nullptr, &mTexture);

		Diligent::TextureViewDesc viewDesc;

		viewDesc.Format = static_cast<Diligent::TEXTURE_FORMAT>(desc.Format);
		viewDesc.NumMipLevels = 0;
		viewDesc.AccessFlags = Diligent::UAV_ACCESS_UNSPECIFIED;
		viewDesc.Flags = mMips == 1 ? Diligent::TEXTURE_VIEW_FLAG_NONE : Diligent::TEXTURE_VIEW_FLAG_ALLOW_MIP_MAP_GENERATION;
		viewDesc.NumDepthSlices = 0;
		viewDesc.NumArraySlices = 0;
		viewDesc.TextureDim = static_cast<Diligent::RESOURCE_DIMENSION>(desc.Type);

		if (desc.Flags & TextureFlags::RenderTarget && !isDepth)
		{
			viewDesc.ViewType = Diligent::TEXTURE_VIEW_RENDER_TARGET;
			mTexture->CreateView(viewDesc, &mRenderTargetView);
		}

		if (desc.Flags & TextureFlags::ShaderInput)
		{
			viewDesc.ViewType = Diligent::TEXTURE_VIEW_SHADER_RESOURCE;
			mTexture->CreateView(viewDesc, &mShaderResourceView);
		}

		if (isDepth)
		{
			viewDesc.ViewType = Diligent::TEXTURE_VIEW_READ_ONLY_DEPTH_STENCIL;
			mTexture->CreateView(viewDesc, &mDepthStencilView);
		}

		auto& formatInfo = diligentDevice->GetTextureFormatInfo(static_cast<Diligent::TEXTURE_FORMAT>(desc.Format));
		mTexelSize = formatInfo.ComponentSize * formatInfo.NumComponents;

		
		if (desc.CreateImGuiTexture)
		{
			mImGuiID = static_cast<ImTextureID>(mShaderResourceView);
		}
	}

	template<typename T>
	bool Texture::ReadData(const TextureReadInfo& info, Buffer<T>& buffer)
	{
		if (~(mFlags & TextureFlags::CPUReadable))
		{
			MULE_LOG_ERROR("Trying to read from texture: {0} that was not configured for reading", mName);
			return false;
		}
		
		auto ctx = mGraphicsDevice->GetMainThreadContext();

		buffer = Buffer(info.Extent.x * info.Extent.y * info.Extent.z * mTexelSize);

		Diligent::Box box;
		box.MinX = info.Offset.x;
		box.MinY = info.Offset.y;
		box.MinZ = info.Offset.z;

		box.MaxX = info.Offset.x + info.Extent.x;
		box.MaxY = info.Offset.y + info.Extent.y;
		box.MaxZ = info.Offset.z + info.Extent.z;

		Diligent::MappedTextureSubresource resource;

		ctx->MapTextureSubresource(mTexture, info.MipLevel, info.Layer, Diligent::MAP_TYPE::MAP_READ,
			Diligent::MAP_FLAG_DO_NOT_WAIT, &box, resource);

		// move data to buffer
		return true;
	}

	Texture::~Texture()
	{

	}

	
}