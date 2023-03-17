#include "mulepch.h"

#include "Mule/ECS/Serializer.h"

#include "Mule/Assets/AssetManager.h"

namespace Mule
{
	void SerializeMaterial(const fs::path& path, Ref<Material> material)
	{
		YAML::Node root;
		
		//	Textures
		root["AlbedoMap"] = material->AlbedoMap;
		root["NormalMap"] = material->NormalMap;
		root["RoughnessMap"] = material->RoughnessMap;
		root["MetalnessMap"] = material->MetalnessMap;
		root["AOMap"] = material->AOMap;
		
		root["AlbedoColor"] = SerializeVec4(material->AlbedoColor);
		root["Transparency"] = material->Transparency;
		root["Roughness"] = material->Roughnes;
		root["Metalness"] = material->Metalness;
		root["Scale"] = SerializeVec2(material->MaterialScale);
		
		std::ofstream file(path.u8string());
		if (!file.is_open())
		{
			LOG_ERR(L"Failed to open file \"{0}\"", path.u8string());
			return;
		}
		file << root;
		file.close();
	}

	Ref<Material> DeSerializeMaterial(const fs::path& path)
	{
		YAML::Node root = YAML::LoadFile(path.u8string());

		Ref<Material> material = MakeRef<Material>();

		AssetManager& manager = AssetManager::Get();

		material->AlbedoMap = root["AlbedoMap"].as<std::string>();
		material->NormalMap = root["NormalMap"].as<std::string>();
		material->RoughnessMap = root["RoughnessMap"].as<std::string>();
		material->MetalnessMap = root["MetalnessMap"].as<std::string>();
		material->AOMap = root["AOMap"].as<std::string>();

		material->AlbedoColor = DeSerializeVec4(root["AlbedoColor"]);

		material->Transparency = root["Transparency"].as<float>();
		material->Roughnes = root["Roughness"].as<float>();
		material->Metalness = root["Metalness"].as<float>();
		material->MaterialScale = DeSerializeVec2(root["Scale"]);

		std::string name = path.filename().u8string();
		material->Name = name.substr(0, name.find_last_of("."));

		manager.TrackAsset(material->Name, material, path, Asset_Material);

		return material;
	}
}