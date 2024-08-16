project "Mule Editor"
	language "C++"
	kind "ConsoleApp"
	location ""
	cppdialect "C++20"
	runtime "Debug"
	staticruntime "Off"

	linkoptions { '/NODEFAULTLIB:"libcpmtd.lib"' }
	buildoptions {'/MP'}

	
	defines
	{
		"GLM_FORCE_INTRINSICS",
		"GLM_ENABLE_EXPERIMENTAL",
		"YAML_CPP_STATIC_DEFINE",
		"NOMINMAX"
	}

	includedirs 
	{
		"../Mule Engine/include",
        "../Mule Engine/include/Mule",
        "../Application/include",
        "include/Mule Engine",
		includes,
		"src"
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