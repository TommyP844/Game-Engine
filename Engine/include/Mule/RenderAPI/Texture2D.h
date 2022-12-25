#pragma once

#include "ITexture.h"

#include <vulkan/vulkan.hpp>

namespace Mule
{
	class Texture2D : public ITexture
	{
	public:
		Texture2D();
		~Texture2D();

		Ref<Texture2D> Create(void* data, int width, int height, TextureFormat format);

		virtual int GetWidth() const override;
		virtual int GetHeight() const override;
		virtual int GetDepth() const override = 0;

		virtual void SetData(void* data, int length) override;
		virtual void* GetData() override;

		virtual Ref<ITexture> Load(const std::filesystem::path& filepath) override;
		virtual Ref<ITexture> LoadAsync(const std::filesystem::path& filepath) override;
		virtual void Save(const std::filesystem::path& filepath) override;
		virtual void SaveAsync(const std::filesystem::path& filepath) override;

	private:
		int mWidth;
		int mHeight;
	};
}