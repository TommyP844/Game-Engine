#pragma once

#include "GraphicsDevice.h"
#include "GraphicsTypes.h"
#include "DiligentCore/Graphics/GraphicsEngine/interface/GraphicsTypes.h"
#include "Util/Buffer.h"
#include <glm.hpp>

#include "imgui.h"

namespace Mule
{
	enum TextureType
	{
		Type_1D = Diligent::RESOURCE_DIMENSION::RESOURCE_DIM_TEX_1D,
		Type_2D = Diligent::RESOURCE_DIMENSION::RESOURCE_DIM_TEX_2D,
		Type_3D = Diligent::RESOURCE_DIMENSION::RESOURCE_DIM_TEX_3D,
		Type_1DArray = Diligent::RESOURCE_DIMENSION::RESOURCE_DIM_TEX_1D_ARRAY,
		Type_2DArray = Diligent::RESOURCE_DIMENSION::RESOURCE_DIM_TEX_2D_ARRAY,
		Type_CubeMap = Diligent::RESOURCE_DIMENSION::RESOURCE_DIM_TEX_CUBE,
		Type_CubeMapArray = Diligent::RESOURCE_DIMENSION::RESOURCE_DIM_TEX_CUBE_ARRAY,
	};

	enum TextureFlags
	{
		None,
		RenderTarget,
		ShaderInput,
		CPUWriteable,
		CPUReadable,
		GenerateMips
	};

	struct TextureDescription
	{
		std::string Name;
		uint32_t Width = 1;
		uint32_t Height = 1;
		uint32_t Depth = 1;
		uint32_t Layers = 1;
		TextureFormat Format;
		Samples SampleCount = Samples::SampleCount_1;
		TextureFlags Flags;
		TextureType Type;
		bool CreateImGuiTexture;
	};

	struct TextureReadInfo
	{
		glm::ivec3 Offset;
		glm::ivec3 Extent;
		uint32_t MipLevel;
		uint32_t Layer;
	};

	class Texture
	{
	public:
		static Ref<Texture> Create(WeakRef<GraphicsDevice> device, const TextureDescription& desc);

		uint32_t GetWidth() const { return mWidth; }
		uint32_t GetHeight() const { return mHeight; }
		uint32_t GetDepth() const { return mDepth; }
		uint32_t GetTexelSize() const { return mTexelSize; }

		uint32_t GetLayers() const { return mLayers; }
		uint32_t GetMips() const { return mMips; }

		Samples GetSamples() const { return mSampleCount; }
		TextureType GetTextureType() const { return mType; }
		TextureFormat GetFormat() const { return mFormat; }
		const std::string& GetName() const { return mName; }

		template<typename T>
		bool ReadData(const TextureReadInfo& info, Buffer<T>& buffer);

		ImTextureID GetImGuiID() const { return mImGuiID; }

		Diligent::ITextureView* GetRenderTargetView() const { return mRenderTargetView.RawPtr(); }
		Diligent::ITextureView* GetShaderResourceView() const { return mShaderResourceView.RawPtr(); }
		Diligent::ITextureView* GetDepthView() const { return mDepthStencilView.RawPtr(); }

		~Texture();

	private:
		Texture(WeakRef<GraphicsDevice> device, const TextureDescription& desc);

		std::string mName;
		uint32_t mWidth;
		uint32_t mHeight;
		uint32_t mDepth;
		uint32_t mLayers;
		uint32_t mMips;
		TextureFormat mFormat;
		Samples mSampleCount;
		TextureFlags mFlags;
		TextureType mType;
		uint32_t mTexelSize;

		Diligent::RefCntAutoPtr<Diligent::ITextureView> mRenderTargetView;
		Diligent::RefCntAutoPtr<Diligent::ITextureView> mDepthStencilView;
		Diligent::RefCntAutoPtr<Diligent::ITextureView> mShaderResourceView;
		Diligent::RefCntAutoPtr<Diligent::ITexture> mTexture;
		ImTextureID mImGuiID;

		WeakRef<GraphicsDevice> mGraphicsDevice;
	};
}