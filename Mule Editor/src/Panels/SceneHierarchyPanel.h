#pragma once

#include "Panel.h"
#include <set>

class SceneHierarchyPanel : public Panel
{
public:
	SceneHierarchyPanel(EditorLayer* layer)
		:
		Panel("Scene Hierarchy Panel", layer)
	{}

	virtual void OnAttach() override;
	virtual void OnUpdate(float dt) override;
	virtual void OnDetach() override;
	virtual void OnEvent(Mule::Ref<Mule::Event> event) override;

private:
};

