
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
		glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
		mWindow = glfwCreateWindow(800, 600, title, nullptr, nullptr);
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
			
			event->Keyboard.Pressed = action == GLFW_PRESS || action == GLFW_REPEAT;
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

		
		glfwSetCharCallback(mWindow, [](GLFWwindow* window, unsigned int codepoint) {
			Window* win = (Window*)glfwGetWindowUserPointer(window);
			Ref<WindowEvent> event = Ref<WindowEvent>::Make();
			event->Type = WindowEventType::Char;

			event->Keyboard.Char = (Key)codepoint;

			win->AddEvent(event);
			});

		glfwSetWindowCloseCallback(mWindow, [](GLFWwindow* window) {
			Window* win = (Window*)glfwGetWindowUserPointer(window);
			Ref<WindowEvent> event = Ref<WindowEvent>::Make();
			event->Type = WindowEventType::Close;
			event->Window.CloseRequested = true;

			win->AddEvent(event);
			});

		glfwSetScrollCallback(mWindow, [](GLFWwindow* window, double scrollX, double scrollY) {
			Window* win = (Window*)glfwGetWindowUserPointer(window);
			Ref<WindowEvent> event = Ref<WindowEvent>::Make();
			event->Type = WindowEventType::Scroll;
			event->Mouse.ScrollXOffset = scrollX;
			event->Mouse.ScrollYOffset = scrollY;

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

	int Window::GetWidth() const
	{
		int width;
		glfwGetWindowSize(mWindow, &width, NULL);
		return width;
	}

	int Window::GetHeight() const
	{
		int height;
		glfwGetWindowSize(mWindow, NULL, &height);
		return height;
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