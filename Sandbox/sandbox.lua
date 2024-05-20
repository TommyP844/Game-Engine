project "Sandbox"
	language "C++"
	kind "ConsoleApp"
	location ""
	cppdialect "C++20"
	runtime "Debug"
	staticruntime "Off"
	optimize "Speed"
	floatingpoint "Fast"
	intrinsics "On"

	linkoptions { '/NODEFAULTLIB:"libcpmtd.lib"' }

	
	defines
	{
		"GLM_FORCE_INTRINSICS",
		"GLM_ENABLE_EXPERIMENTAL",
		"YAML_CPP_STATIC_DEFINE"
	}

	includedirs 
	{
		"../Mule Engine/include",
        "../Mule Engine/include/Mule",
        "../Application/include",
        "include/Mule Engine",
		includes
	}

	links
	{
		"Application",
		"Mule Engine",
		"glfw",
		"yaml-cpp"
	}

	files
	{
		"src/**.h",
		"src/**.cpp",
	}

	filter {"configurations:Debug"}
		links { debugLibs }

	filter {"configurations:Release"}
		links { releaseLibs }