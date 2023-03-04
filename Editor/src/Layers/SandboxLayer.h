#pragma once

#include "ILayer.h"

#include "Mule/Mule.h"

class SandBoxLayer : public ILayer
{
public:
	virtual void OnAttach() override;
	virtual void OnUpdate() override;
	virtual void OnDetach() override;

private:



};