@echo off

set cwd=%cd%


if not exist "%cd%/SubModules/DiligentEngine/install" (
	cd %cd%/SubModules/DiligentEngine
	echo Building Diligent Engine...
	set "imgui_path=%cwd:\=/%/SubModules/imgui"
	echo %imgui_path%
	cmake -S . -B ./build/Win64 -G "Visual Studio 17 2022" -A x64 -DDILIGENT_DEAR_IMGUI_PATH=%imgui_path%
	if not %errorlevel% == 0 (
		goto :EOF
	)
	cd build/Win64
	cmake --build . --config Release --target install
	cmake --build . --target install
	echo "Diligent Engine built and installed"
	cd %cwd%
)

premake5 vs2022

:EOF