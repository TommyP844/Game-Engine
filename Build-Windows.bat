@echo off
if not exist "SubModules/Assimp/Assimp.sln" (
	cd "SubModules/Assimp"
	cmake CMakeLists.txt
	cmake --build .
	cmake  --build . --config Release
	cd ../..
	
	:: These need to be post build commands
	:: copy /Y "SubModules\Assimp\bin\Debug\assimp-vc143-mtd.dll" "Editor\bin\Debug\assimp-vc143-mtd.dll"
	:: copy /Y "SubModules\Assimp\bin\Debug\assimp-vc143-mtd.pdb" "Editor\bin\Debug\assimp-vc143-mt
	:: 	
	:: copy /Y "SubModules\Assimp\bin\Release\assimp-vc143-mt.dll" "Editor\bin\Release\assimp-vc143-mt.dll"
	:: copy /Y "SubModules\Assimp\bin\Release\assimp-vc143-mt.pdb" "Editor\bin\Release\assimp-vc143-mt.pdb"
)

cd "SubModules/bgfx"
..\bx\tools\bin\windows\genie --with-examples vs2017
cd ../..
premake5.exe vs2022
pause