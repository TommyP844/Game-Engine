project "ImGui"
	language "C++"
	kind "StaticLib"
	location ""
	cppdialect "C++20"

	runtime "Debug"
	staticruntime "off"
	optimize "Speed"
	floatingpoint "Fast"
	intrinsics "On"

	linkoptions { '/NODEFAULTLIB:"libcpmtd.lib"' }

	includedirs 
	{
		""
	}

	links
	{
	}

	files
	{
		"*.h",
		"*.cpp",
	}
