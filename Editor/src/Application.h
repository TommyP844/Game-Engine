#pragma once 

#include "Mule/Mule.h"

#include "Layers/ILayer.h"

#include <vector>

#include "AppData.h"

class Application
{
public:
	void Init();
	void Run();

	void PushLayer(Mule::Ref<ILayer> layer);
	void PopLayer();

private:
	std::vector<Mule::Ref<ILayer>> mLayers;
	Mule::Ref<Mule::Window> mWindow;
	AppData mAppData;
	
};