#pragma once

#include "ILayer.h"

#include "Mule/Mule.h"

class SandBoxLayer : public ILayer
{
public:
	SandBoxLayer(AppData& appData) : ILayer(appData) {}

	virtual void OnAttach() override;
	virtual void OnUpdate() override;
	virtual void OnDetach() override;

private:



};