#pragma once 

class ILayer
{
public:
	virtual ~ILayer() {}

	virtual void OnAttach() = 0;
	virtual void OnUpdate() = 0;
	virtual void OnDetach() = 0;
};