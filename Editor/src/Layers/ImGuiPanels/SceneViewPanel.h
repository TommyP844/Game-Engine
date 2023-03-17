#pragma once

#include "IPanel.h"

class SceneViewPanel : public IPanel
{
public:
	SceneViewPanel(AppData& data);
	~SceneViewPanel(){}

	virtual void OnAttach() override;
	virtual void OnUpdate() override;
	virtual void OnDetach() override;
};
