#pragma once

#include "Mule.h"
#include "ApplicationData.h"

class EditorLayer;

class Panel
{
public:
	Panel(const std::string& name, Mule::Ref<ApplicationData> appData)
		:
		mName(name),
		mIsOpen(true),
		mAppData(appData)
	{}
	~Panel(){}

	virtual void OnAttach() = 0;
	virtual void OnUpdate(float dt) = 0;
	virtual void OnDetach() = 0;
	virtual void OnImGuiRender() = 0;
	virtual void OnEvent(Mule::Ref<Mule::Event> event) = 0;

	const std::string& Name() const { return mName; }

protected:
	bool mIsOpen;
	Mule::Ref<ApplicationData> mAppData;
private:
	std::string mName;
};

