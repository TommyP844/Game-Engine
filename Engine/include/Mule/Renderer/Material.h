#pragma once


#include <glm/glm.hpp>

#include "Texture.h"

#include <filesystem>

namespace fs = std::filesystem;

namespace Mule
{
	class Material
	{
	public:
		Material();
		~Material() {}

		static Ref<Material> GetDefault();

		std::string Name;

		glm::vec4 AlbedoColor;

		std::string AlbedoMap;
		std::string NormalMap;
		std::string RoughnessMap;
		std::string MetalnessMap;
		std::string AOMap;

		float Transparency;
		glm::vec2 MaterialScale;
		float Metalness;
		float Roughnes;
	};
}