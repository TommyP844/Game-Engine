#pragma once

#include "Mule/Ref.h"

#include "GLFW/glfw3.h"

namespace Mule {

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

	private:
		GLFWwindow* mWindow;

	};

}