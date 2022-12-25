project "Mule Engine"
	language "C++"
	kind "StaticLib"
	location ""
	cppdialect "C++17"
	
	pchheader "mulepch.h"
	pchsource "src/mulepch.cpp"

	defines
	{
		"YAML_CPP_STATIC_DEFINE"
	}

	includedirs 
	{
		"include",
		"C:/VulkanSDK/1.3.231.1/Include",
		"../SubModules/entt/src",
		"../Submodules/glm",
		"../Submodules/Yaml/include",
		"../SubModules/glfw/include"
	}

	links
	{
		"yaml-cpp",
		"glfw",
		"C:/VulkanSDK/1.3.231.1/Lib/vulkan-1.lib"
	}
	
	files
	{
		"include/**.h",
		"include/**.inl",
		"src/**.cpp"
	}

	filter "configurations:Debug"
      defines { "DEBUG" }

   filter "configurations:Release"
      defines { "RELEASE" }
	