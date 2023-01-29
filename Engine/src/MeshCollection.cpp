#include "mulepch.h"
#include "Mule/Renderer/MeshCollection.h"

namespace Mule
{
	Ref<MeshCollection> MeshCollection::Create(const std::vector<Ref<Mesh>>& meshes)
	{
		Ref<MeshCollection> collection = MakeRef<MeshCollection>();
		collection->mMeshes = meshes;
		return collection;
	}
}