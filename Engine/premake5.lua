project "Mule Engine"
	language "C++"
	kind "StaticLib"
	location ""
	cppdialect "C++17"
	
	pchheader "mulepch.h"
	pchsource "src/mulepch.cpp"

	defines
	{
		"YAML_CPP_STATIC_DEFINE",
		"GLM_FORCE_SSE2"
	}

	includedirs 
	{
		"include",
		"C:/VulkanSDK/1.3.231.1/Include",
		"../SubModules/entt/src",
		"../Submodules/glm",
		"../Submodules/Yaml/include",
		"../SubModules/glfw/include",
		"../SubModules/stb",
		"../SubModules/Assimp/include",
		"../Submodules/ImGui/ImGui"
	}

	links
	{
		"yaml-cpp",
		"glfw",
		"C:/VulkanSDK/1.3.231.1/Lib/vulkan-1.lib",
		"ImGui"
	}
	
	files
	{
		"include/**.h",
		"include/**.inl",
		"src/**.cpp"
	}

	filter "configurations:Debug"
      defines { "DEBUG" }
	  links
	  {
		  "../SubModules/Assimp/lib/Debug/assimp-vc143-mtd.lib"
	  }

   filter "configurations:Release"
      defines { "RELEASE" }
	  links
	  {
		  "../SubModules/Assimp/lib/Release/assimp-vc143-mt.lib"
	  }
	  optimize "Speed"
	  floatingpoint "Fast"
	  intrinsics "On"
	