#pragma once

#include "Panel.h"

class AssetBrowser : public Panel
{
public:
	AssetBrowser(EditorLayer* layer)
		:
		Panel("Asset Browser", layer)
	{}

	virtual void OnAttach() override;
	virtual void OnUpdate(float dt) override;
	virtual void OnDetach() override;
	virtual void OnEvent(Mule::Ref<Mule::Event> event) override;

private:
};
