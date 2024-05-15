project "Application"
	language "C++"
	kind "StaticLib"
	location ""
	cppdialect "C++20"

	staticruntime "off"
	optimize "Speed"
	floatingpoint "Fast"
	intrinsics "On"
	vectorextensions "SSE4.2"

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
		libs
	}

	files
	{
		"include/**.h",
		"include/**.inl",
		"src/**.cpp"
	}