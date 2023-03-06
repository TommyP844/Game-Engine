#pragma once

#include "../Ref.h"
#include "../Window/Window.h"

namespace Mule
{
	namespace Renderer
	{
		bool Init(Ref<Window> window);

		void NewImGuiFrame();

		void EndImGuiFrame();

		void AdvanceFrame();

		void Shutdown();
	}
}