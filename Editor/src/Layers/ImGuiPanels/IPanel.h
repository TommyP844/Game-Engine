#pragma once

#include <string>
#include "../../AppData.h"

class IPanel
{
public:
	IPanel(const std::string& name, AppData& appData) : mName(name), mAppData(appData) {}
	virtual ~IPanel(){}

	virtual void OnAttach() = 0;
	virtual void OnUpdate() = 0;
	virtual void OnDetach() = 0;

	const std::string& GetName() const { return mName; }

protected:
	AppData& mAppData;
private:
	std::string mName;
};
