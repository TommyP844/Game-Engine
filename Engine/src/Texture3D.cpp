#include "mulepch.h"

#include "Mule\RenderAPI\Texture3D.h"

namespace Mule
{
	Texture3D::Texture3D()
		:
		ITexture(TextureType::TEXTURE_3D)
	{
	}
	
	Texture3D::~Texture3D()
	{
	}

	Ref<Texture3D> Texture3D::Create(void* data, int width, int height, int depth, TextureFormat format)
	{
		Ref<Texture3D> texture = MakeRef<Texture3D>();
		texture->mWidth = width;
		texture->mHeight = height;
		texture->mDepth = depth;
		texture->mFormat = format;

		return texture;
	}
	
	int Texture3D::GetWidth() const
	{
		return mWidth;
	}
	
	int Texture3D::GetHeight() const
	{
		return mHeight;
	}
	
	int Texture3D::GetDepth() const
	{
		return mDepth;
	}
	
	void Texture3D::SetData(void* data, int length)
	{
	}
	
	void* Texture3D::GetData()
	{
		return nullptr;
	}
}