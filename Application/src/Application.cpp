
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
	const int WIDTH = 1600;
	const int HEIGHT = 900;

	Mule::Log::Initialize(std::cout);

	Mule::Window::Init();
	mWindow = Mule::Window::Create("Title", WIDTH, HEIGHT);

	mGraphicsDevice = Mule::GraphicsDevice::Create(Mule::GraphicsAPI::Vulkan, mWindow);

	Mule::ImGuiMuleCreateInfo imGuiInfo{};
	imGuiInfo.Window = mWindow;
	imGuiInfo.Context = mGraphicsDevice->GetMainThreadContext();
	imGuiInfo.Device = mGraphicsDevice;
	imGuiInfo.InitialWidth = WIDTH;
	imGuiInfo.InitialHeight = HEIGHT;

	mImGuiContext = Mule::ImGuiMuleContext::Create(imGuiInfo);

}

void Application::Run()
{
	while (!mWindow->ShouldClose() && mAppIsRunning)
	{
		std::vector<Mule::Ref<Mule::WindowEvent>> events = mWindow->PollEvents();

		for (auto event : events)
		{
			mImGuiContext->AddEvent(event);
		}

		if (!mLayers.empty())
		{
			auto layer = mLayers.top();
			layer->OnUpdate();
			mImGuiContext->BeginFrame();
			layer->OnImGuiRender();
			mImGuiContext->Render();
		}

		mGraphicsDevice->SwapBuffers();
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

