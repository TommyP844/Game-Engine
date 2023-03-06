#pragma once

#include "Mule/Ref.h"

#include "GLFW/glfw3.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"
#include <glm/glm.hpp>

#include <queue>

namespace Mule {

	struct WindowEvent
	{
		WindowEvent() = default;
		WindowEvent(const WindowEvent& other) = default;
		float TimeStamp;
		int mx, my;
		int mscroll;
		bool leftPressed, middlePressed, rightPressed;
		int character;
		int key;
		bool pressed;
	};

	class Window
	{
	public:
		Window() = default;
		~Window();
		static void Init();
		static void Shutdown();

		static Ref<Window> Create(int width, int height, const char* title);

		void PollEvents();

		GLFWwindow* GetWindowPtr() const { return mWindow; }

		glm::ivec2 GetSize() const;

		HWND GetWin32Handle();

		void SubmitEvent(WindowEvent& event) { mEvents.push(event); }

		const std::queue<WindowEvent>& GetEvents() const { return mEvents; }

		WindowEvent GetEventAndPop();

		void InitEventSystemWithImGui();

		int GetEventQueueSize() const { return mEvents.size(); }

		glm::ivec2 GetCursorPos() const;

		int GetMouseButtons() const;

		int GetMouseScroll() const;

	private:
		std::queue<WindowEvent> mEvents;
		GLFWwindow* mWindow;

	};

}