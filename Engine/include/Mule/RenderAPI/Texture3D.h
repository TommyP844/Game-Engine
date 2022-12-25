#pragma once

#include "ITexture.h"

namespace Mule
{
	class Texture3D : public ITexture
	{
	public:
		Texture3D();
		~Texture3D();

		Ref<Texture3D> Create(void* data, int width, int height, int depth, TextureFormat format);

		virtual int GetWidth() const override;
		virtual int GetHeight() const override;
		virtual int GetDepth() const override;

		virtual void SetData(void* data, int length) override;
		virtual void* GetData() override;

		virtual Ref<ITexture> Load(const std::filesystem::path& filepath) override = 0;
		virtual Ref<ITexture> LoadAsync(const std::filesystem::path& filepath) override = 0;
		virtual void Save(const std::filesystem::path& filepath) override = 0;
		virtual void SaveAsync(const std::filesystem::path& filepath) override = 0;

	private:
		int mWidth;
		int mHeight;
		int mDepth;
	};
}
