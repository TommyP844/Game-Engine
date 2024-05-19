#pragma once

#include <string>
#include <filesystem>

namespace fs = std::filesystem;

static std::string GenerateNewFilename(const std::string& filename, const fs::path& directory) {
	std::string newFilename = filename.substr(0, filename.find_last_of("."));
	std::string ext = "";
	if (filename.find(".") != std::string::npos)
	{
		ext = filename.substr(filename.find_last_of("."));
	}
	int counter = 1;
	std::stringstream ss;
	do {
		ss.clear();
		ss << newFilename << " (" << counter << ")";
		newFilename = ss.str();
		counter++;
	} while (fs::exists(directory / (ss.str() + ext)));
	return ss.str() + ext;
}

enum class FileType
{
	Unknown,
	Directory,
	Scene,
	Texture,
	Mesh,
	Shader,
	Material
};

static FileType TypeFromPath(const fs::path& path);
static FileType TypeFromPath(const fs::directory_entry& entry)
{
	if (entry.is_directory())
		return FileType::Directory;
	return TypeFromPath(entry.path());
}

FileType TypeFromPath(const fs::path& path)
{
	std::string ext = path.extension().string();
	std::set<std::string> textureExtensions = { ".jpeg", ".jpg", ".tga", ".bmp", ".png", ".hdr"};
	std::set<std::string> modelExtensions = { ".obj", ".dae", ".fbx", ".glb" };

	if (ext == ".scene")
		return FileType::Scene;

	if (textureExtensions.find(ext) != textureExtensions.end())
		return FileType::Texture;
	if (modelExtensions.find(ext) != modelExtensions.end())
		return FileType::Mesh;
	if (ext == ".mshader")
		return FileType::Shader;
	if (ext == ".mmaterial")
		return FileType::Material;

	return FileType::Unknown;
}

