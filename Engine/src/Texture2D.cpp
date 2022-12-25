#include "mulepch.h"

#include "Mule/RenderAPI/Texture2D.h"

namespace Mule
{
	Texture2D::Texture2D()
		:
		ITexture(TextureType::TEXTURE_2D)
	{
	}

	Texture2D::~Texture2D()
	{
	}

	Ref<Texture2D> Texture2D::Create(void* data, int width, int height, TextureFormat format)
	{
		Ref<Texture2D> texture = MakeRef<Texture2D>();
		texture->mWidth = width;
		texture->mHeight = height;
		texture->mFormat = format;

		return texture;
	}

	Ref<Texture2D> Texture2D::Create()
	{
		return Ref<Texture2D>();
	}
	
	int Texture2D::GetWidth() const
	{
		return mWidth;
	}
	
	int Texture2D::GetHeight() const
	{
		return mHeight;
	}

	void Texture2D::SetData(void* data, int length)
	{
	}

	void* Texture2D::GetData()
	{
		return nullptr;
	}
	
	Ref<ITexture> Texture2D::Load(const std::filesystem::path& filepath)
	{
		return Ref<ITexture>();
	}
	
	Ref<ITexture> Texture2D::LoadAsync(const std::filesystem::path& filepath)
	{
		return Ref<ITexture>();
	}
	
	void Texture2D::Save(const std::filesystem::path& filepath)
	{
	}

	void Texture2D::SaveAsync(const std::filesystem::path& filepath)
	{
	}
}