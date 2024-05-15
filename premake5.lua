workspace "Mule"
    configurations { "Debug", "Release" }
	architecture "x64"

    dir = os.getcwd();

    includes = {
        dir .. "/SubModules/entt/single_include",
        dir .. "/SubModules/glm/glm",
        dir .. "/SubModules/yaml-cpp/include",
        dir .. "/SubModules/glfw/include"
    }

    libs = {
        dir .. "/Submodules/yaml-cpp/bin/Debug/yaml-cpp.lib",
        dir .. "/SubModules/glfw/bin/Debug/glfw.lib"
    }
 
	startproject "Sandbox"

    include "Mule Engine/Mule Engine.lua"
    include "Application/Application.lua"
    include "Sandbox/sandbox.lua"
    group "SubModules"
        include "SubModules/yaml-cpp/yaml.lua"
        include "SubModules/glfw/premake5.lua"
    group ""


    