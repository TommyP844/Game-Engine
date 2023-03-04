#include "Application.h"

#include "Layers/ImGuiLayer.h"
#include "Layers/SandboxLayer.h"

void Application::Init()
{
	Mule::Window::Init();
	mWindow = Mule::Window::Create(800, 600, "Mule");

	PushLayer(Mule::MakeRef<ImGuiLayer>());
	PushLayer(Mule::MakeRef<SandBoxLayer>());
}

void Application::Run()
{	
	while (true)
	{
		mWindow->PollEvents();


		for (auto layer : mLayers)
		{
			layer->OnUpdate();
		}
	}

	while (mLayers.size() > 0)
	{
		PopLayer();
	}

	//Mule::Internal::ThreadPool::Shutdown();
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
