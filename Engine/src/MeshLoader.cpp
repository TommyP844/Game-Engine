#include "mulepch.h"
#include "Mule/Renderer/MeshLoader.h"

#include "Mule/Util/Log.h"

#include <bgfx/bgfx.h>

#include <glm/glm.hpp>

namespace Mule
{
	Assimp::Importer MeshLoader::sImporter;

	Ref<MeshCollection> MeshLoader::LoadMesh(const fs::path& filepath)
	{
		int flags = aiProcess_GenSmoothNormals | aiProcess_Triangulate;
		
		sImporter.SetPropertyFloat("#AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE", 80.f);
		const aiScene* scene = sImporter.ReadFile(filepath.u8string(), flags);
		LOG_MSG(L"Loading mesh {0}", filepath.u8string());
		if (!scene)
		{
			LOG_ERR(L"Failed to load mesh: {0}", filepath.u8string());
			return nullptr;
		}

		Ref<MeshCollection> collection = MakeRef<MeshCollection>();

		std::string name = filepath.filename().u8string();
		name = name.substr(0, name.find_last_of("."));

		collection->Name = name;

		for (int i = 0; i < scene->mNumMeshes; i++)
		{
			Ref<Mesh> mesh = LoadMesh(scene->mMeshes[i]);
			collection->Meshes.push_back(mesh);
			//Ref<Material> material = LoadMaterial(scene->mMaterials[scene->mMeshes[i]->mMaterialIndex]);
		}

		sImporter.FreeScene();

		return collection;
	}

	Ref<Mesh> MeshLoader::LoadMesh(const aiMesh* mesh)
	{
		struct Vertex
		{
			glm::vec3 Pos;
			glm::vec3 Normal;
			glm::vec2 TexCoord;
		};

		std::vector<Vertex> vertices;
		vertices.reserve(mesh->mNumVertices);

		for (int i = 0; i < mesh->mNumVertices; i++)
		{
			aiVector3D vertex = mesh->mVertices[i];
			aiVector3D normal(0, 0, 0);
			if (mesh->mNormals != NULL) normal = mesh->mNormals[i];
			aiVector3D texCoord(0, 0, 0);
			if(mesh->mTextureCoords[0] != NULL) texCoord = mesh->mTextureCoords[0][i];
			aiColor4D color(0, 0, 0, 0);
			if(mesh->mColors[0] != NULL) color = mesh->mColors[0][i];

			Vertex v;
			v.Pos = { vertex.x, vertex.y, vertex.z };
			v.Normal = { normal.x, normal.y, normal.z };
			v.TexCoord = { texCoord.x, texCoord.y };
			//v.Color = { color.r, color.g, color.b, color.a };
			
			vertices.emplace_back(v);
		}

		std::vector<unsigned int> indices;
		indices.reserve(mesh->mNumFaces * 3);

		for (int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			if (face.mNumIndices != 3)
			{
				LOG_WARN(L"Face does not have 3 indicies");
			}
			for (int j = 0; j < 3; j++)
			{
				indices.emplace_back(face.mIndices[j]);
			}
		}

		bgfx::VertexLayout layout;
		layout.begin()
			.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
			.add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
			.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
			.end();

		Ref<Mesh> loadedMesh = Mesh::Create(vertices, indices, layout, "");

		return loadedMesh;
	}
	
	Ref<Material> MeshLoader::LoadMaterial(const aiMaterial* material)
	{
		
		return Ref<Material>();
	}
}