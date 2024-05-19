#pragma once

#include "Panel.h"
#include <thread>

class ComponentPanel : public Panel
{
public:
	ComponentPanel(EditorLayer* layer)
		:
		Panel("Component Panel", layer)
	{}

	virtual void OnAttach() override;
	virtual void OnUpdate(float dt) override;
	virtual void OnDetach() override;
	virtual void OnEvent(Mule::Ref<Mule::Event> event) override;

private:
	std::thread mLoadModelThread;
};

