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

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		window->mWindow = glfwCreateWindow(width, height, title, NULL, NULL);


		return window;
	}

	void Window::PollEvents()
	{
		glfwPollEvents();
	}


}
