#include "Application.h"

#include "Layers/ImGuiLayer.h"
#include "Layers/SandboxLayer.h"

#include "imgui.h"

void Application::Init()
{
	Mule::Window::Init();
	mWindow = Mule::Window::Create(800, 600, "Mule");

	Mule::Renderer::Init(mWindow);
	mWindow->InitEventSystemWithImGui(); // must be called after imgui is setup

	PushLayer(Mule::MakeRef<ImGuiLayer>());
	PushLayer(Mule::MakeRef<SandBoxLayer>());
}

void Application::Run()
{	
	while (true)
	{
		mWindow->PollEvents();

		Mule::Renderer::NewImGuiFrame();

		for (auto layer : mLayers)
		{
			layer->OnUpdate();
		}

		ImGui::Begin("test");

		char buffer[24] = { 0 };
		ImGui::InputText("Test", buffer, 24);

		ImGui::End();

		Mule::Renderer::EndImGuiFrame();

		Mule::Renderer::AdvanceFrame();
	}

	while (mLayers.size() > 0)
	{
		PopLayer();
	}

	//Mule::Internal::ThreadPool::Shutdown();
	Mule::Renderer::Shutdown();
	Mule::Window::Shutdown();
}

void Application::PushLayer(Mule::Ref<ILayer> layer)
{
	layer->OnAttach();
	mLayers.push_back(layer);
}

void Application::PopLayer()
{
	mLayers.back()->OnDetach();
	mLayers.pop_back();
}
