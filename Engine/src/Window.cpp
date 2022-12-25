#include "mulepch.h"

#include "Mule/Window/Window.h"


namespace Mule {
	
	Window::~Window()
	{
		glfwDestroyWindow(mWindow);
	}

	void Window::Init()
	{
		glfwInit();
	}

	void Window::Shutdown()
	{
		glfwTerminate();
	}

	Ref<Window> Window::Create(int width, int height, const char* title)
	{
		Ref<Window> window = MakeRef<Window>();

		window->mWindow = glfwCreateWindow(width, height, title, NULL, NULL);

		glfwSetCharCallback(window->mWindow, [](GLFWwindow* window, unsigned int c) {

			});

		return window;
	}

	void Window::PollEvents()
	{
		glfwPollEvents();
	}


}
