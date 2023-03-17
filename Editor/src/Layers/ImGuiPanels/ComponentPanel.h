#pragma once

#include "IPanel.h"

class ComponentPanel : public IPanel
{
public:
	ComponentPanel(AppData& appData) : IPanel("Component Panel", appData) {}
	~ComponentPanel();

	virtual void OnAttach() override;
	virtual void OnUpdate() override;
	virtual void OnDetach() override;
};

