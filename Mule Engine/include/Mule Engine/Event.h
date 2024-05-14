#pragma once

namespace Mule
{
	enum class EventType
	{
		WindowEvent
	};

	class Event
	{
	public:
		Event(EventType type)
			:
			mEventType(type),
			mHandled(false)
		{}

		void SetHandled() { mHandled = true; }
		EventType Type() const { return mEventType; }
		bool Handled() const { return mHandled; }
	private:
		EventType mEventType;
		bool mHandled;
	};
}