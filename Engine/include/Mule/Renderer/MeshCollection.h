#pragma once

#include <vector>
#include "Mule/Renderer/Mesh.h"

namespace Mule
{
	struct MeshCollection
	{
		std::string Name;
		std::vector<Ref<Mesh>> Meshes;
	};
}