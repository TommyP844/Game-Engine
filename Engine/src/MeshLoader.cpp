#include "mulepch.h"
#include "Mule/MeshLoader.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "Mule/Renderer/Mesh.h"
#include "Mule/Renderer/VertexDefault.h"

#include "Mule/Util/Log.h"

namespace Mule
{
	glm::vec4 ToGlmVec4(const aiColor4D& vec)
	{
		return { vec.r, vec.g, vec.b, vec.a };
	}

	glm::vec3 ToGlmVec3(const aiVector3D& vec)
	{
		return { vec.x, vec.y, vec.z };
	}

	glm::vec2 ToGlmVec2(const aiVector2D& vec)
	{
		return { vec.x, vec.y };
	}

	static Assimp::Importer sImporter;

	Ref<MeshCollection> MeshLoader::LoadMesh(const std::filesystem::path& filepath)
	{
		LOG_MSG(L"Loading mesh \"{0}\"", filepath.u8string());
		sImporter.SetPropertyFloat(AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE, 80.f);
		int flags = aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_GenSmoothNormals;

		const aiScene* scene = sImporter.ReadFile(filepath.u8string(), flags);

		std::vector<Ref<Mesh>> meshes;

		for (int i = 0; i < scene->mNumMeshes; i++)
		{
			const aiMesh* mesh = scene->mMeshes[i];

			std::vector<Vertex> vertices;
			LOG_MSG(L"Num vertices in mesh {0}", mesh->mNumVertices);
			for (int j = 0; j < mesh->mNumVertices; j++)
			{
				Vertex vertex;

				vertex.Position = ToGlmVec3(mesh->mVertices[j]);
				vertex.Normal = ToGlmVec3(mesh->mNormals[j]);

				if (mesh->HasTangentsAndBitangents())
				{
					vertex.Tangent = ToGlmVec3(mesh->mTangents[j]);
					vertex.BiTangent = ToGlmVec3(mesh->mBitangents[j]);
				}
				vertex.Color = mesh->HasVertexColors(0) ? ToGlmVec4(mesh->mColors[0][j]) : glm::vec4(0.f);
				vertex.UV = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };

				vertices.push_back(vertex);
			}

			std::vector<unsigned int> indices;

			for (int j = 0; j < mesh->mNumFaces; j++)
			{
				const aiFace face = mesh->mFaces[j];
				for (int k = 0; k < face.mNumIndices; k++)
				{
					indices.push_back(face.mIndices[k]);
				}
			}

			auto vertexBuffer = VertexBuffer::Create(&vertices[0], vertices.size(), sizeof(Vertex));
			auto indexBuffer = IndexBuffer::Create(&indices[0], indices.size());

			auto muleMesh = Mesh::Create(vertexBuffer, indexBuffer);
			meshes.push_back(muleMesh);
		}

		sImporter.FreeScene();

		LOG_SUCCESS(L"Mesh loaded \"{0}\"", filepath.u8string());

		return MeshCollection::Create(meshes);
	}
}