project "Application"
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
        "include",
		includes
	}

	links
	{
		"Mule Engine",
        "opengl32.lib",
		"glfw",
		"yaml-cpp"
	}

	files
	{
		"include/**.h",
		"include/**.inl",
		"src/**.cpp"
	}

	filter {"configurations:Debug"}
		links { debugLibs }

	filter {"configurations:Debug"}
		links { releaseLibs }