
#include "Application.h"


Application::Application(const std::string& name)
{
}

Application::~Application()
{
	Mule::Window::Shutdown();
}

void Application::Setup()
{
	Mule::Log::Initialize(std::cout);

	Mule::Window::Init();
	mWindow = Mule::Window::Create();
}

void Application::Run()
{
	while (mAppIsRunning)
	{
		mWindow->PollEvents();

		
		if (!mLayers.empty())
		{
			auto layer = mLayers.top();
			layer->OnUpdate(0.f);
		}
	}
}

void Application::PushLayer(Mule::Ref<ILayer> layer)
{
	layer->OnAttach();
	mLayers.push(layer);
}

void Application::PopLayer()
{
	if (mLayers.empty())
	{
		MULE_LOG_ERROR("Failed to pop layer, layer stack empty");
	}

	auto layer = mLayers.top();
	layer->OnDetach();
	mLayers.pop();
}

