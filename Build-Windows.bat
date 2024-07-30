@echo off

set cwd=%cd%


if not exist "%cd%/SubModules/DiligentEngine/build/Win64/install" (
	cd %cd%/SubModules/DiligentEngine
	echo Building Diligent Engine...
	cmake -S . -B ./build/Win64 -G "Visual Studio 17 2022" -A x64 -DDILIGENT_DEAR_IMGUI_PATH=C:/Dev/Mule-Engine/Submodules/imgui -DDILIGENT_BUILD_SAMPLES=FALSE
	cd build/Win64
	echo "Diligent Engine built and installed"
	cd %cwd%
)

premake5 vs2022

:EOF