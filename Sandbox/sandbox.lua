project "Sandbox"
	language "C++"
	kind "ConsoleApp"
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
        "../Mule Engine/include/Mule Engine",
        "../App/inlude",
        "include/Mule Engine"
	}

	links
	{
		"App"
	}

	files
	{
		"src/**.h",
		"src/**.cpp",
	}