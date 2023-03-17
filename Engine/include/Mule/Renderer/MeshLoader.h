#pragma once

#include "Mule/Renderer/Mesh.h"
#include "Mule/Renderer/Material.h"
#include "Mule/Renderer/MeshCollection.h"

#include <assimp/version.h>
#include <assimp/config.h>
#include <assimp/importerdesc.h>

#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/material.h>
#include <assimp/texture.h>
#include <assimp/BaseImporter.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

#include <filesystem>

namespace fs = std::filesystem;

namespace Mule
{

	class MeshLoader
	{
	public:
		static Ref<MeshCollection> LoadMesh(const fs::path& filepath);

	private:
		static Ref<Mesh> LoadMesh(const aiMesh* mesh);
		static Ref<Material> LoadMaterial(const aiMaterial* material);

		static Assimp::Importer sImporter;
	};
}
