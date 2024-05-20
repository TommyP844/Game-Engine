#pragma once

#include "Panel.h"
#include <set>

class SceneHierarchyPanel : public Panel
{
public:
	SceneHierarchyPanel(Mule::Ref<ApplicationData> appData)
		:
		Panel("Scene Hierarchy Panel", appData)
	{}

	virtual void OnAttach() override;
	virtual void OnUpdate(float dt) override;
	virtual void OnDetach() override;
	virtual void OnEvent(Mule::Ref<Mule::Event> event) override;
	virtual void OnImGuiRender() override;

private:
};

