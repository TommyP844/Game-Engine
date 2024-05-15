#pragma once 

#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"
#include "Ref.h"


#include <vector>
#include "WindowEvent.h"

namespace Mule
{
	class Window
	{
	public:
		~Window(){}

		static bool Init();
		static void Shutdown();
		
		static Ref<Window> Create();

		GLFWwindow* GLFWWindow() const { return mWindow; }
		void SwapBuffers();
		bool ShouldClose();

		Ref<WindowEvent> PollEvents();

		Ref<WindowEvent> mEvent;
	private:
		Window(){}

		GLFWwindow* mWindow;
	};
}