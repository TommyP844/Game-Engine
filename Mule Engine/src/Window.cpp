
#include "Window.h"

namespace Mule
{
	bool Window::Init()
	{
		if (glfwInit() != GLFW_TRUE)
		{
			return false;
		}
		return true;
	}

	void Window::Shutdown()
	{
		glfwTerminate();
	}

	Ref<Window> Window::Create()
	{
		Window* win = new Window();
		win->mEvent = Ref<WindowEvent>::Make();

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
		GLFWwindow* window = glfwCreateWindow(800, 600, "Title", nullptr, nullptr);
		glfwSetWindowUserPointer(window, win);
		glfwMakeContextCurrent(window);
		glfwSwapInterval(0); // disable vsync

		glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
			Window* win = (Window*)glfwGetWindowUserPointer(window);

			win->mEvent->Mousebuttons[button] = action == GLFW_PRESS ? 1 : 0;
			});

		
		glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
			Window* win = (Window*)glfwGetWindowUserPointer(window);

			win->mEvent->KeyPressed[key] = action == GLFW_PRESS ? 1 : 0;
			});

		glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
			Window* win = (Window*)glfwGetWindowUserPointer(window);

			win->mEvent->MouseX = xpos;
			win->mEvent->MouseY = ypos;
			});
		
		glClearColor(0, 0, 0, 1);

		win->mWindow = window;
		return Ref<Window>(win);
	}

	void Window::SwapBuffers()
	{
		glfwSwapBuffers(mWindow);
	}

	bool Window::ShouldClose()
	{
		return glfwWindowShouldClose(mWindow);
	}

	Ref<WindowEvent> Window::PollEvents()
	{
		glfwPollEvents();
		return mEvent;
	}
}