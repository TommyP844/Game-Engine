project "Mule Engine"
	language "C++"
	kind "StaticLib"
	location ""
	cppdialect "C++17"
	
	pchheader "mulepch.h"
	pchsource "src/mulepch.cpp"

	includedirs 
	{
		"include",
		"../SubModules/entt/src",
		"../Submodules/glm",
		"../Submodules/Yaml/include"
	}

	links
	{
		"yaml-cpp"
	}
	
	files
	{
		"include/**.h",
		"include/**.inl",
		"src/**.cpp"
	}
	