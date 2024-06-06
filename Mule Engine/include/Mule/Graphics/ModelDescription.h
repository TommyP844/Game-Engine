#pragma once

#include "Ref.h"
#include "Asset/Asset.h"
#include "Graphics/Mesh.h"

namespace Mule
{
	struct MeshNode
	{
		glm::mat4 OffsetMatrix;
		std::vector<AssetHandle> Meshes;
		std::vector<MeshNode> Children;
	};


	class ModelDescription : public Asset
	{
	public:
		static Ref<ModelDescription> Create(AssetHandle handle, const fs::path& filepath)
		{
			return Ref<ModelDescription>(new ModelDescription(handle, filepath));
		}
		static Ref<ModelDescription> Create(const fs::path& filepath)
		{
			return Ref<ModelDescription>(new ModelDescription(filepath));
		}

		MeshNode Root;

		~ModelDescription(){}
	private:
		ModelDescription(AssetHandle handle, const fs::path& filepath)
			:
			Asset(handle, filepath, AssetType::Model)
		{}
		
		ModelDescription(const fs::path& filepath)
			:
			Asset(Asset::GenerateHandle(), filepath, AssetType::Model)
		{}
	};
}