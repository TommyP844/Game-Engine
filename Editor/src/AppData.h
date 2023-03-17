#pragma once

#include "Mule/Mule.h"

#include <string>
#include <map>
#include <filesystem>

namespace fs = std::filesystem;

struct AppData
{
	Mule::Ref<Mule::Scene> ActiveScene;
	std::vector<Mule::Entity> SelectedEntities;
	Mule::Ref<Mule::Window> Window;
	fs::path AssetDir;
	std::map<std::string, Mule::Ref<Mule::Texture>> EditorTextures;
};