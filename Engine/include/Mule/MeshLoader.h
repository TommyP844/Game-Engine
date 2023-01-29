#pragma once

#include <filesystem>

#include "Mule/Ref.h"
#include "Mule/Renderer/MeshCollection.h"

namespace Mule
{

	class MeshLoader
	{
	public:
		static Ref<MeshCollection> LoadMesh(const std::filesystem::path& filepath);
	};
}