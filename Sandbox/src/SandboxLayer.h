#pragma once

#include "Mule Engine.h"
#include "../../App/include/ILayer.h"

class SandboxLayer : public ILayer
{
public:
	SandboxLayer(Mule::Ref<ApplicationData> appData, const std::string& name)
		:
		ILayer(appData, name)
	{}

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnUpdate(float dt = 0.f) override;
	virtual void OnUIRender(float dt = 0.f) override;
};