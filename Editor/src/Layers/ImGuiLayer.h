#pragma once

#include "Mule/Mule.h"
#include "ILayer.h"
#include "ImGuiPanels/IPanel.h"
#include <vector>

class ImGuiLayer : public ILayer
{
public:
	virtual void OnAttach() override;
	virtual void OnUpdate() override;
	virtual void OnDetach() override;

	void PushPanel(Mule::Ref<IPanel> panel);

	void RemovePanel(Mule::Ref<IPanel> panel);

private:
	std::vector<Mule::Ref<IPanel>> mPanels;

};