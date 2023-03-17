#pragma once 

#include "../AppData.h"

class ILayer
{
public:
	ILayer(AppData& appData) : mAppData(appData) {}
	virtual ~ILayer() {}

	virtual void OnAttach() = 0;
	virtual void OnUpdate() = 0;
	virtual void OnDetach() = 0;

protected:
	AppData& mAppData;
};