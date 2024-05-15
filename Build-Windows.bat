@echo off

if not exist "%cd%/SubModules/DiligentEngine/install" (
	echo "Building Diligent Engine..."

	echo "Diligent Engine built and installed"
)

premake5 vs2022