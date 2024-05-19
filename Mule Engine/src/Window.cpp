
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

	Ref<Window> Window::Create(const char* title, int width, int height)
	{
		return Ref<Window>(new Window(title, width, height));
	}

	Window::Window(const char* title, int width, int height)
	{
		mWindow = glfwCreateWindow(width, height, title, nullptr, nullptr);
		glfwSetWindowUserPointer(mWindow, this);
		glfwMakeContextCurrent(mWindow);
		glfwSwapInterval(0); // disable vsync

		glfwSetMouseButtonCallback(mWindow, [](GLFWwindow* window, int button, int action, int mods) {
			Window* win = (Window*)glfwGetWindowUserPointer(window);
			Ref<WindowEvent> event = Ref<WindowEvent>::Make();
			event->Type = WindowEventType::MouseButton;

			event->Mouse.Button = (MouseButton)button;
			event->Mouse.Pressed = action == GLFW_PRESS;
				

			win->AddEvent(event);
			});


		glfwSetKeyCallback(mWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
			Window* win = (Window*)glfwGetWindowUserPointer(window);
			Ref<WindowEvent> event = Ref<WindowEvent>::Make();
			event->Type = WindowEventType::Key;
			
			event->Keyboard.Pressed = action == GLFW_PRESS;
			event->Keyboard.Key = (Key)key;

			win->AddEvent(event);
			});

		glfwSetCursorPosCallback(mWindow, [](GLFWwindow* window, double xpos, double ypos) {
			Window* win = (Window*)glfwGetWindowUserPointer(window);
			Ref<WindowEvent> event = Ref<WindowEvent>::Make();
			event->Type = WindowEventType::MouseMove;

			event->Mouse.MouseX = xpos;
			event->Mouse.MouseY = ypos;

			win->AddEvent(event);
			});

		glfwSetWindowSizeCallback(mWindow, [](GLFWwindow* window, int width, int height) {
			Window* win = (Window*)glfwGetWindowUserPointer(window);
			Ref<WindowEvent> event = Ref<WindowEvent>::Make();
			event->Type = WindowEventType::WindowResize;

			event->Window.Width = width;
			event->Window.Height = height;

			win->AddEvent(event);
		});
	}

	HWND Window::GetWin32Window()
	{
		return glfwGetWin32Window(mWindow);
	}

	void Window::SwapBuffers()
	{
		glfwSwapBuffers(mWindow);
	}

	bool Window::ShouldClose()
	{
		return glfwWindowShouldClose(mWindow);
	}

	void Window::AddEvent(Ref<WindowEvent> event)
	{
		mFrameEvents.push_back(event);
	}

	const std::vector<Ref<WindowEvent>>& Window::PollEvents()
	{
		mFrameEvents.clear();
		glfwPollEvents();
		return mFrameEvents;
	}
}