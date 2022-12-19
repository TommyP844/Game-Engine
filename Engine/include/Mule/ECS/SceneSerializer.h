#pragma once

#include <filesystem>

#include "yaml-cpp/yaml.h"

#include "Mule/Ref.h"
#include "Mule/ECS/Scene.h"

namespace Mule {
	namespace SceneSerializer {

		enum SerializerFlags
		{
			HumanReadable = 0,
			Binary
		};

		static void Serialize(const std::filesystem::path& filepath, Ref<Scene> scene, SerializerFlags flags = SerializerFlags::HumanReadable);

		static Ref<Scene> DeSerialize(const std::filesystem::path& filepath, SerializerFlags flags = SerializerFlags::HumanReadable);

	}
}