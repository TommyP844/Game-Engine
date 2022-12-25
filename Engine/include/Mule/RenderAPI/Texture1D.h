#pragma once

#include "ITexture.h"

namespace Mule
{
	class Texture1D : public ITexture
	{
	public:
		Texture1D();
		~Texture1D();

		// TODO: determine how we want to create these textures
		Ref<Texture1D> Create(void* data, int width, TextureFormat format);

		virtual int GetWidth() const override;
		virtual int GetHeight() const override = 0;
		virtual int GetDepth() const override = 0;

		virtual void SetData(void* data, int length) override;
		virtual void* GetData() override;

		virtual Ref<ITexture> Load(const std::filesystem::path& filepath) override = 0;
		virtual Ref<ITexture> LoadAsync(const std::filesystem::path& filepath) override = 0;
		virtual void Save(const std::filesystem::path& filepath) override = 0;
		virtual void SaveAsync(const std::filesystem::path& filepath) override = 0;

	private:
		int mWidth;
	};
}