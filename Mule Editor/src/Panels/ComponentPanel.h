#pragma once

#include "Panel.h"
#include <thread>

class ComponentPanel : public Panel
{
public:
	ComponentPanel(Mule::Ref<ApplicationData> appData)
		:
		Panel("Component Panel", appData)
	{}

	virtual void OnAttach() override;
	virtual void OnUpdate(float dt) override;
	virtual void OnDetach() override;
	virtual void OnEvent(Mule::Ref<Mule::Event> event) override;
	virtual void OnImGuiRender() override;

private:
	std::thread mLoadModelThread;
};

