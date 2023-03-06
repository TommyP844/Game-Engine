#pragma once
#include "IPanel.h"

class ScenePanel : public IPanel
{
public:
	ScenePanel();
	~ScenePanel(){}

	virtual void OnAttach() override;
	virtual void OnUpdate() override;
	virtual void OnDetach() override;
};

