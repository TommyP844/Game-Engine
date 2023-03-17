#include "mulepch.h"

#include "Mule/Renderer/Material.h"
#include "Mule/Util/Log.h"

namespace Mule
{
	Material::Material()
		:
		Name(),
		AlbedoColor({0, 0, 0, 1}),
		AlbedoMap(),
		NormalMap(),
		RoughnessMap(),
		MetalnessMap(),
		AOMap(),
		Transparency(1.f),
		MaterialScale({1.f, 1.f}),
		Metalness(0.5f),
		Roughnes(0.5f)
	{
	}

	Ref<Material> Material::GetDefault()
	{
		static Ref<Material> material;

		if (!material)
		{
			material = MakeRef<Material>();
			material->Name = "Default";
			material->AlbedoColor = { 1, 1, 1, 1 };
			material->Transparency = 0.f;
		}

		return material;
	}
}