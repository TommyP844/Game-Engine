project "Mule Engine"
	language "C++"
	kind "StaticLib"
	location ""
	cppdialect "C++17"
	
	pchheader "mulepch.h"
	pchsource "src/mulepch.cpp"

	staticruntime "on"
	runtime "Debug"


	defines
	{
		"YAML_CPP_STATIC_DEFINE",
		"GLM_FORCE_SSE2"
	}

	includedirs 
	{
		"include",
		"../SubModules/entt/src",
		"../Submodules/glm",
		"../Submodules/Yaml/include",
		"../SubModules/glfw/include",
		"../SubModules/stb",
		"../SubModules/Assimp/include",
		"../Submodules/ImGui/ImGui",
		"../SubModules/bgfx/include",
		"../SubModules/bx/include"
	}

	links
	{
		"yaml-cpp",
		"glfw",
		"ImGui"
	}
	
	files
	{
		"include/**.h",
		"include/**.inl",
		"src/**.cpp"
	}

	filter "configurations:Debug"
      defines 
	  {
		  "DEBUG",
		  "BX_CONFIG_DEBUG"
	  }
	  links
	  {
		  "../SubModules/Assimp/lib/Debug/assimp-vc143-mtd.lib",
		  "../SubModules/bgfx/.build/win64_vs2017/bin/bgfxDebug.lib",
		  "../SubModules/bgfx/.build/win64_vs2017/bin/bxDebug.lib",
		  "../SubModules/bgfx/.build/win64_vs2017/bin/bimgDebug.lib",
	  }
	  staticruntime "on"
	  runtime "Debug"

   filter "configurations:Release"
      defines { "RELEASE" }
	  links
	  {
		  "../SubModules/Assimp/lib/Release/assimp-vc143-mt.lib",
		  "../SubModules/bgfx/.build/win64_vs2017/bin/bgfxRelease.lib",
		  "../SubModules/bgfx/.build/win64_vs2017/bin/bxRelease.lib",
		  "../SubModules/bgfx/.build/win64_vs2017/bin/bimgRelease.lib",
	  }
	  optimize "Speed"
	  floatingpoint "Fast"
	  intrinsics "On"
	