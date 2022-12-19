workspace "Mule"
	configurations { "Debug", "Release" }
	architecture "x64"
	
	-- SubModules
	include "SubModules/Yaml/premake5.lua"
	include "SubModules/glfw/premake5.lua"

	-- My Projects
	include "Engine/premake5.lua"


