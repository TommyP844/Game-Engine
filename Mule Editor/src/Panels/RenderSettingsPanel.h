#pragma once

#include "Panels/Panel.h"

class RenderSettingsPanel : public Panel
{
public:
	RenderSettingsPanel(EditorLayer* editor)
		:
		Panel("Render Settings", editor)
	{}

	virtual void OnAttach() override;
	virtual void OnUpdate(float dt) override;
	virtual void OnDetach() override;
	virtual void OnEvent(Mule::Ref<Mule::Event> event) override;
};