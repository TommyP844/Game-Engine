#pragma once

#include "Mule/Ref.h"
#include "Mule/Renderer/Mesh.h"

#include <vector>

namespace Mule
{
	class MeshCollection
	{
	public:
		MeshCollection(){}
		~MeshCollection(){}

		static Ref<MeshCollection> Create(const std::vector<Ref<Mesh>>& meshes);

		const std::vector<Ref<Mesh>> GetMeshes() const { return mMeshes; }

	private:
		std::vector<Ref<Mesh>> mMeshes;
	};
}
