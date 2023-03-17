#pragma once

#include "EngineEvent.h"

#include <vector>

namespace Mule
{
	class EngineEventManager
	{
	public:
		void Push(const EngineEvent& event);

	private:
		std::vector<EngineEvent> mEvents;
	};
}