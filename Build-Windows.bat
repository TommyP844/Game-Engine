@echo off

set cwd=%cd%


if not exist "%cd%/SubModules/DiligentEngine/install" (
	cd %cd%/SubModules/DiligentEngine
	echo "Building Diligent Engine..."
	cmake -S . -B ./build/Win64 -G "Visual Studio 17 2022" -A x64
	cd build/Win64
	cmake --build . --config Release --target install
	cmake --build . --target install
	echo "Diligent Engine built and installed"
	cd %cwd%
)

premake5 vs2022