project "Mule Editor"
	language "C++"
	kind "ConsoleApp"
	location ""
	cppdialect "C++17"

	defines
	{
		"YAML_CPP_STATIC_DEFINE",
		"GLM_FORCE_SSE2",
	}
	

	includedirs 
	{
		"../Engine/include",

		-- SubModules
		"../SubModules/entt/src",
		"../Submodules/glm",
		"../Submodules/Yaml/include",
		"../SubModules/glfw/include",
		"C:/VulkanSDK/1.3.231.1/Include",
		"../Submodules/ImGui/ImGui",

		"../SubModules/bgfx/include",
		"../SubModules/bx/include",
		"../SubModules/stb",
		"../SubModules/Assimp/include"
	}

	links
	{
		"Mule Engine"
	}
	
	files
	{
		"src/**.h",
		"src/**.cpp"
	}


	filter "configurations:Debug"
		defines 
		{
			"DEBUG",
			"BX_CONFIG_DEBUG"
		}
		postbuildcommands 
		{
			'copy /Y "$(SolutionDir)SubModules\\Assimp\\bin\\Debug\\assimp-vc143-mtd.dll" "$(SolutionDir)Editor\\bin\\Debug\\assimp-vc143-mtd.dll"',
			'copy /Y "$(SolutionDir)SubModules\\Assimp\\bin\\Debug\\assimp-vc143-mtd.pdb" "$(SolutionDir)Editor\\bin\\Debug\\assimp-vc143-mtd.pdb"'
		
		}
		links
		{
		  "../SubModules/bgfx/.build/win64_vs2017/bin/bgfxDebug.lib",
		  "../SubModules/bgfx/.build/win64_vs2017/bin/bxDebug.lib",
		  "../SubModules/bgfx/.build/win64_vs2017/bin/bimgDebug.lib",
		}
		staticruntime "on"
		runtime "Debug"

   filter "configurations:Release"
		defines { "RELEASE" }
		postbuildcommands 
		{
				'copy /Y "$(SolutionDir)SubModules\\Assimp\\bin\\Release\\assimp-vc143-mt.dll" "$(SolutionDir)Editor\\bin\\Release\\assimp-vc143-mt.dll"',
				'copy /Y "$(SolutionDir)SubModules\\Assimp\\bin\\Release\\assimp-vc143-mt.pdb" "$(SolutionDir)Editor\\bin\\Release\\assimp-vc143-mt.pdb"'
		}
		links
		{
		  "../SubModules/bgfx/.build/win64_vs2017/bin/bgfxRelease.lib",
		  "../SubModules/bgfx/.build/win64_vs2017/bin/bxRelease.lib",
		  "../SubModules/bgfx/.build/win64_vs2017/bin/bimgRelease.lib",
		}
		optimize "Speed"
		floatingpoint "Fast"
		intrinsics "On"
	

