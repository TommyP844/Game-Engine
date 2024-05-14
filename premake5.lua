workspace "Mule"
    configurations { "Debug", "Release" }
	architecture "x64" 
 
	startproject "Sandbox"

    include "Mule Engine/Mule Engine.lua"
    include "App/app.lua"
    include "Sandbox/sandbox.lua"


    