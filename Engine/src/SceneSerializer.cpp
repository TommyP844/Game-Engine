#include "mulepch.h"

#include "Mule/ECS/SceneSerializer.h"

void Mule::SceneSerializer::Serialize(const std::filesystem::path& filepath, Ref<Scene> scene, SerializerFlags flags)
{
}

Ref<Scene> Mule::SceneSerializer::DeSerialize(const std::filesystem::path& filepath, SerializerFlags flags)
{
    return Ref<Scene>();
}
