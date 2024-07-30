#pragma once 

#if _WIN32
#include <Windows.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#endif

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
		
		static Ref<Window> Create(const char* title, int width, int height);

		GLFWwindow* GLFWWindow() const { return mWindow; }
		HWND GetWin32Window();
		void SwapBuffers();
		bool ShouldClose();

		int GetWidth() const;
		int GetHeight() const;

		void AddEvent(Ref<WindowEvent> event);
		const std::vector<Ref<WindowEvent>>& PollEvents();
	private:
		Window(const char* title, int width, int height);

		std::vector<Ref<WindowEvent>> mFrameEvents;
		GLFWwindow* mWindow;
	};
}