#pragma once

#include <string>
#include <filesystem>
#include <regex>

#ifdef _WIN32
#include <windows.h>
#include <shobjidl.h> // For IFileDialog
#endif

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
	std::set<std::string> modelExtensions = { ".gltf" };

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

static bool IsFilenameValid(const fs::path& filename) {
	// Define the regex for a valid filename
	std::regex filenameRegex("^[\\w.-]+$");
	return std::regex_match(filename.string(), filenameRegex);
}

static char const* SelectFolderDialog(
    const char const* aTitle, /* NULL or "" */
    const char const* aDefaultPath) /* NULL or "" */
{
    char* result = NULL;
#ifdef _WIN32
    static char lBuff[MAX_PATH];
    HRESULT hr;
    IFileDialog* pFileDialog = nullptr;

    // Initialize COM library
    hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hr))
    {
        // Create the FileOpenDialog object
        hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, IID_IFileDialog, reinterpret_cast<void**>(&pFileDialog));
        if (SUCCEEDED(hr))
        {
            // Set the options on the dialog
            DWORD dwOptions;
            hr = pFileDialog->GetOptions(&dwOptions);
            if (SUCCEEDED(hr))
            {
                // Enable folder picker option
                pFileDialog->SetOptions(dwOptions | FOS_PICKFOLDERS);

                // Set the default folder if provided
                if (aDefaultPath && aDefaultPath[0])
                {
                    wchar_t wDefaultPath[MAX_PATH];
                    MultiByteToWideChar(CP_ACP, 0, aDefaultPath, -1, wDefaultPath, MAX_PATH);

                    IShellItem* pDefaultPathItem = nullptr;
                    hr = SHCreateItemFromParsingName(wDefaultPath, nullptr, IID_PPV_ARGS(&pDefaultPathItem));
                    if (SUCCEEDED(hr))
                    {
                        pFileDialog->SetFolder(pDefaultPathItem);
                        pDefaultPathItem->Release();
                    }
                }

                // Show the dialog
                hr = pFileDialog->Show(nullptr);
                if (SUCCEEDED(hr))
                {
                    // Get the result
                    IShellItem* pItem;
                    hr = pFileDialog->GetResult(&pItem);
                    if (SUCCEEDED(hr))
                    {
                        // Get the display name of the folder
                        PWSTR pszFilePath = nullptr;
                        hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                        if (SUCCEEDED(hr))
                        {
                            // Convert wide string to multibyte string
                            size_t convertedChars = 0;
                            wcstombs_s(&convertedChars, lBuff, pszFilePath, _TRUNCATE);
                            CoTaskMemFree(pszFilePath);
                            result = lBuff;
                        }
                        pItem->Release();
                    }
                }
            }
            pFileDialog->Release();
        }
        CoUninitialize();
    }
#endif
    return result;
}

