project "Mule Engine"
	language "C++"
	kind "StaticLib"
	location ""
	
	includedirs "include"
	
	files
	{
		"include/**.h",
		"src/**.cpp"
	}
	