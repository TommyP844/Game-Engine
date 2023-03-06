#include "mulepch.h"

#include "Mule/Window/Window.h"

#include "backends/imgui_impl_glfw.h"



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
		glfwSetWindowUserPointer(window->mWindow, window.get());

		return window;
	}

	void Window::PollEvents()
	{
		glfwPollEvents();
	}

	glm::ivec2 Window::GetSize() const
	{
		glm::ivec2 s;
		glfwGetWindowSize(mWindow, &s.x, &s.y);
		return s;
	}

	HWND Window::GetWin32Handle()
	{
		HWND hwnd = glfwGetWin32Window(mWindow);
		return hwnd;
	}

	WindowEvent Window::GetEventAndPop()
	{
		if (mEvents.empty())
			return WindowEvent();

		WindowEvent recent = mEvents.front();
		mEvents.pop();
		return recent;
	}

	void Window::InitEventSystemWithImGui()
	{
		glfwSetMouseButtonCallback(mWindow, [](GLFWwindow* window, int button, int action, int mods) {
			Window* win = (Window*)glfwGetWindowUserPointer(window);
			WindowEvent event{};

			switch (button)
			{
			case GLFW_MOUSE_BUTTON_LEFT:
				event.leftPressed = (action == GLFW_PRESS);
				break;
			case GLFW_MOUSE_BUTTON_MIDDLE:
				event.middlePressed = (action == GLFW_PRESS);
				break;
			case GLFW_MOUSE_BUTTON_RIGHT:
				event.rightPressed = (action == GLFW_PRESS);
				break;
			}

			win->SubmitEvent(event);

			ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);

			});


		glfwSetCursorPosCallback(mWindow, [](GLFWwindow* window, double x, double y) {
			Window* win = (Window*)glfwGetWindowUserPointer(window);
			WindowEvent event{};
			event.mx = x;
			event.my = y;
			win->SubmitEvent(event);
			ImGui_ImplGlfw_CursorPosCallback(window, x, y);
			});

		glfwSetCharCallback(mWindow, [](GLFWwindow* window, unsigned int codepoint) {
			Window* win = (Window*)glfwGetWindowUserPointer(window);
			WindowEvent event{};
			event.character = codepoint;
			win->SubmitEvent(event);
			ImGui_ImplGlfw_CharCallback(window, codepoint);
			});

		glfwSetScrollCallback(mWindow, [](GLFWwindow* window, double x, double y) {
			Window* win = (Window*)glfwGetWindowUserPointer(window);
			WindowEvent event{};
			event.mscroll = y;
			win->SubmitEvent(event);
			ImGui_ImplGlfw_ScrollCallback(window, x, y);
			});

		glfwSetKeyCallback(mWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
			ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
			});

		
		glfwSetFramebufferSizeCallback(mWindow, [](GLFWwindow* window, int width, int height) {
			ImGuiIO& io = ImGui::GetIO();
			io.DisplaySize.x = width;
			io.DisplaySize.y = height;
			
		
			});

	}

	glm::ivec2 Window::GetCursorPos() const
	{
		glm::dvec2 pos;
		glfwGetCursorPos(mWindow, &pos.x, &pos.y);
		return glm::ivec2(pos.x, pos.y);
	}

	int Window::GetMouseButtons() const
	{
		int left = glfwGetMouseButton(mWindow, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS ? 1 : 0;
		int middle = glfwGetMouseButton(mWindow, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS ? 2 : 0;
		int right = glfwGetMouseButton(mWindow, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS ? 4 : 0;
		return left | right | middle;
	}

	int Window::GetMouseScroll() const
	{
		
		return 0;
	}


}
