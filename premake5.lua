include "util.lua"

workspace "Mule"
    configurations { "Debug", "Release" }
	architecture "x64"

    dir = os.getcwd();

    includes = {
        dir .. "/SubModules/entt/single_include",
        dir .. "/SubModules/glm/glm",
        dir .. "/SubModules/yaml-cpp/include",
        dir .. "/SubModules/glfw/include",
        dir .. "/SubModules/DiligentEngine/install/include",
        dir .. "/SubModules/DiligentEngine/DiligentTools/ThirdParty/imgui",
        os.getenv("VULKAN_SDK") .. "/include"
    }

    debugLibs = {
        dir .. "/SubModules/DiligentEngine/install/lib/DiligentCore/Debug/glslangd.lib",
        dir .. "/SubModules/DiligentEngine/install/lib/DiligentCore/Debug/GraphicsEngineVk_64d.lib",
        dir .. "/SubModules/DiligentEngine/install/lib/DiligentCore/Debug/SPIRVd.lib",
        dir .. "/SubModules/DiligentEngine/install/lib/DiligentCore/Debug/SPIRV-Tools.lib",
        dir .. "/SubModules/DiligentEngine/install/lib/DiligentCore/Debug/spirv-cross-cored.lib",
        dir .. "/SubModules/DiligentEngine/install/lib/DiligentCore/Debug/spirv-cross-glsld.lib",
        dir .. "/SubModules/DiligentEngine/install/lib/DiligentCore/Debug/OSDependentd.lib",
        dir .. "/SubModules/DiligentEngine/install/lib/DiligentCore/Debug/GenericCodeGend.lib",
        dir .. "/SubModules/DiligentEngine/install/lib/DiligentCore/Debug/SPIRV-Tools-opt.lib",
        dir .. "/SubModules/DiligentEngine/install/lib/DiligentCore/Debug/Archiver_64d.lib",
        dir .. "/SubModules/DiligentEngine/install/lib/DiligentCore/Debug/MachineIndependentd.lib",
        dir .. "/SubModules/DiligentEngine/install/lib/DiligentCore/Debug/DiligentCore.lib",
        dir .. "/SubModules/DiligentEngine/install/lib/DiligentTools/Debug/DiligentTools.lib"

        
    }

 
	startproject "Mule Editor"

    include "Mule Engine/Mule Engine.lua"
    include "Application/Application.lua"
    --include "Sandbox/sandbox.lua"
    include "Mule Editor/editor.lua"
    group "SubModules"
        include "SubModules/yaml-cpp/yaml.lua"
        include "SubModules/glfw/premake5.lua"
    group ""