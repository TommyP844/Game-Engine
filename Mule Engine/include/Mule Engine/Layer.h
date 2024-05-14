#pragma once

#include <string>
#include "Layer.h"

namespace Mule
{
	class Layer
	{
	public:
		Layer(){}
		Layer(const std::string& name)
			:
			mName(name)
		{}

		virtual ~Layer(){}

		virtual void OnAttach() = 0;
		virtual void OnUpdate(float dt) = 0;
		virtual void OnDestroy() = 0;
		virtual void OnEvent(Mule::Ref<Mule::Event> event) = 0;

		const std::string& Name() const { return mName; }
	private:
		std::string mName;
	};
}
