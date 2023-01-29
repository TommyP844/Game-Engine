workspace "Mule"
	configurations { "Debug", "Release" }
	architecture "x64"

	startproject "Mule Editor"
	
	-- SubModules
	include "SubModules/Yaml/premake5.lua"
	include "SubModules/glfw/premake5.lua"
	include "SubModules/ImGui/ImGui"

	-- My Projects
	include "Engine/premake5.lua"
	include "Editor/premake5.lua"


