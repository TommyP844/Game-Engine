#pragma once

#include "Mule/Ref.h"

namespace Mule
{

	enum class TextureType : int
	{
		TEXTURE_1D,
		TEXTURE_2D,
		TEXTURE_3D,
		TEXTURE_HDR,
		TEXTURE_CUBEMAP
	};

	enum class TextureFormat : int
	{
		FORMAT_R8,
		FORMAT_RG8,
		FORMAT_RGB8,
		FORMAT_RGBA8,

		FORMAT_R32F,
		FORMAT_RG32F,
		FORMAT_RGB32F,
		FORMAT_RGBA32F,

		FORMAT_R16F,
		FORMAT_RG16F,
		FORMAT_RGB16F,
		FORMAT_RGBA16F,

		FORMAT_R32I,
		FORMAT_RG32I,
		FORMAT_RGB32I,
		FORMAT_RGBA32I

	};

	class Texture
	{
	public:
		Texture(){}
		~Texture(){}

		static Ref<Texture> CreateTexture1D(void* data, int width, TextureFormat format);
		static Ref<Texture> CreateTexture2D(void* data, int width, int height, TextureFormat format);
		static Ref<Texture> LoadTexture2D(const std::filesystem::path& filepath);
		static Ref<Texture> LoadTexture2DAsync(const std::filesystem::path& filepath);
		static Ref<Texture> CreateTexture3D(void* data, int width, int height, int depth, TextureFormat format);

	private:

	};
}
