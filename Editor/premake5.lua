project "Mule Editor"
	language "C++"
	kind "ConsoleApp"
	location ""
	cppdialect "C++17"

	defines
	{
		"YAML_CPP_STATIC_DEFINE"
	}
	

	includedirs 
	{
		"../Engine/include",

		-- SubModules
		"../SubModules/entt/src",
		"../Submodules/glm",
		"../Submodules/Yaml/include",
		"../SubModules/glfw/include"
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
      defines { "DEBUG" }

   filter "configurations:Release"
      defines { "RELEASE" }
	