#pragma once

#include "Panel.h"
#include <set>

class SceneHierarchyPanel : public Panel
{
public:
	SceneHierarchyPanel(Mule::Ref<EditorState> editorState)
		:
		Panel("Scene Hierarchy Panel", editorState)
	{}

	virtual void OnAttach() override;
	virtual void OnUpdate(float dt) override;
	virtual void OnDetach() override;
	virtual void OnEvent(Mule::Ref<Mule::Event> event) override;
	virtual void OnImGuiRender() override;

private:
};

