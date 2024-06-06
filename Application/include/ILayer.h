#pragma once

#include <string>

#include "ApplicationData.h"

class ILayer
{
public:
	ILayer(Mule::WeakRef<ApplicationData> appData, const std::string& name) : mName(name), mApplicationData(appData) {}
	virtual ~ILayer(){}

	virtual void OnAttach() = 0;
	virtual void OnDetach() = 0;
	virtual void OnUpdate(float dt = 0.f) = 0;
	virtual void OnImGuiRender(float dt = 0.f) = 0;
	virtual void OnEvent(Mule::Ref<Mule::Event> event) = 0;

	const std::string& GetName() const { return mName; }

protected:
	Mule::WeakRef<ApplicationData> mApplicationData;
private:
	std::string mName;
};