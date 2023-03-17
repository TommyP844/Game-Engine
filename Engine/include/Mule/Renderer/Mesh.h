#pragma once

#include "bgfx/bgfx.h"
#include "Mule/Ref.h"
#include "Mule/Util/Log.h"

#include <vector>

namespace Mule
{
	class Mesh
	{
	public:
		Mesh(){}
		~Mesh();
		Mesh(const Mesh& other) = default;

		template<typename T, typename I>
		static Ref<Mesh> Create(const std::vector<T>& vertices, const std::vector<I>& indices, bgfx::VertexLayout layout, const std::string& defaultMaterial = std::string());

		void Submit();

		const std::string& GetDefaultMaterialName() const { return mDeafultMaterial; }
	private:
		std::string mDeafultMaterial;
		bgfx::VertexBufferHandle mVertexHandle;
		bgfx::IndexBufferHandle mIndexHandle;
	};

	template<typename T, typename I>
	inline Ref<Mesh> Mesh::Create(const std::vector<T>& vertices, const std::vector<I>& indices, bgfx::VertexLayout layout, const std::string& defaultMaterial)
	{
		unsigned char* v = new unsigned char[vertices.size() * sizeof(T)];
		memcpy(v, vertices.data(), vertices.size() * sizeof(T));

		I* i = new I[indices.size()];
		memcpy(i, indices.data(), indices.size() * sizeof(I));

		bgfx::ReleaseFn rel = [](void* ptr, void* userData) {
			delete[] ptr;
		};

		Ref<Mesh> mesh = MakeRef<Mesh>();
		mesh->mDeafultMaterial = defaultMaterial;
		mesh->mVertexHandle = bgfx::createVertexBuffer(bgfx::makeRef(&v[0], vertices.size() * sizeof(T), rel), layout);
		if (indices.size() > 3)
			mesh->mIndexHandle = bgfx::createIndexBuffer(bgfx::makeRef(&i[0], indices.size() * sizeof(I), rel), sizeof(I) == 4 ? BGFX_BUFFER_INDEX32 : 0);

		if (!bgfx::isValid(mesh->mVertexHandle))
		{
			LOG_ERR(L"Failed to create valid vertex buffer");
		}

		if (!bgfx::isValid(mesh->mIndexHandle))
		{
			LOG_ERR(L"Failed to create valid index buffer");
		}

		return mesh;
	}
}