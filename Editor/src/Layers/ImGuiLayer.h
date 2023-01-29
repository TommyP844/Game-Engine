#pragma once

#include "ILayer.h"

class ImGuiLayer : public ILayer
{
public:
	virtual void OnAttach() override;
	virtual void OnUpdate() override;
	virtual void OnDetach() override;

};