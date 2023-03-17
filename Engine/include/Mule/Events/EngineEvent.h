#pragma once


namespace Mule
{
	enum EngineEventType
	{
		EngineEvent_WindowResized
	};

	class EngineEvent
	{
	public:
		EngineEventType Type;
		bool Handled;
	};
}