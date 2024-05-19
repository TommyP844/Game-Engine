
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


	Mule::RenderPassDescription renderPassDesc;
	renderPassDesc.Attachments.push_back({Mule::TextureFormat::RGBA8_SRGB, Mule::Samples::SampleCount_1});
	renderPassDesc.DepthAttachment = { Mule::TextureFormat::Depth32F, Mule::Samples::SampleCount_1};
	renderPassDesc.Name = "Main";
	renderPassDesc.Context = mGraphicsDevice->GetMainThreadContext();
	renderPassDesc.SubPasses.push_back({
		{},
		{0},
		true
		});

	mRenderPass = Mule::RenderPass::Create(mGraphicsDevice, renderPassDesc);

	Mule::FrameBufferDescription desc;
	desc.Name = "Main";
	desc.Width = WIDTH;
	desc.Height = HEIGHT;
	desc.RenderPass = mRenderPass;
	desc.BuildForSwapChain = true;

	mFrameBuffer = Mule::FrameBuffer::Create(mGraphicsDevice, desc);


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

		mImGuiContext->BeginFrame();
		ImGui::Begin("Test");
		ImGui::End();
		mImGuiContext->Render();

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

