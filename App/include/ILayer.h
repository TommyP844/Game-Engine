#pragma once

#include <string>

#include "ApplicationData.h"

class ILayer
{
public:
	ILayer(Mule::Ref<ApplicationData> appData, const std::string& name) : mName(name) {}
	virtual ~ILayer(){}

	virtual void OnAttach() = 0;
	virtual void OnDetach() = 0;
	virtual void OnUpdate(float dt = 0.f) = 0;
	virtual void OnUIRender(float dt = 0.f) = 0;

	const std::string& GetName() const { return mName; }

private:
	std::string mName;
	Mule::Ref<ApplicationData> mApplicationData;
};