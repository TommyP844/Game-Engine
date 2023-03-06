#pragma once

#include <string>

class IPanel
{
public:
	IPanel(const std::string& name) : mName(name) {}
	virtual ~IPanel(){}

	virtual void OnAttach() = 0;
	virtual void OnUpdate() = 0;
	virtual void OnDetach() = 0;

	const std::string& GetName() const { return mName; }

private:
	std::string mName;
};
