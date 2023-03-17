#include "mulepch.h"
#include "Mule/Renderer/Mesh.h"
#include "Mule/Util/Log.h"

#include <glm/glm.hpp>

namespace Mule
{
	Mesh::~Mesh()
	{
		bgfx::destroy(mVertexHandle);
		if (mIndexHandle.idx != bgfx::kInvalidHandle)
			bgfx::destroy(mIndexHandle);
	}

	void Mesh::Submit()
	{
		bgfx::setVertexBuffer(0, mVertexHandle);
		if(mIndexHandle.idx != bgfx::kInvalidHandle)
			bgfx::setIndexBuffer(mIndexHandle);
	}
}