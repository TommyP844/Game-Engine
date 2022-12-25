#include "mulepch.h"
#include "Mule\RenderAPI\Texture1D.h"


namespace Mule
{
    Texture1D::Texture1D()
        :
        ITexture(TextureType::TEXTURE_1D)
    {
    }
    Texture1D::~Texture1D()
    {
    }
    Ref<Texture1D> Texture1D::Create(void* data, int width, TextureFormat format)
    {
        Ref<Texture1D> texture = MakeRef<Texture1D>();
        texture->mWidth = width;
        texture->mFormat = format;

        return texture;
    }
    int Texture1D::GetWidth() const
    {
        return mWidth;
    }
    void Texture1D::SetData(void* data, int length)
    {
    }
    void* Texture1D::GetData()
    {
        return nullptr;
    }
}
