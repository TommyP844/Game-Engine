
#include "Application.h"


Application::Application(const std::string& name)
{
}

Application::~Application()
{
	ImGui::DestroyContext(mApplicationData->ImGuiContext);
	Mule::ImGuiRenderer::Shutdown();
	Mule::Window::Shutdown();
}

void Application::Setup()
{
	Mule::Log::Initialize(std::cout);

	Mule::Window::Init();
	mWindow = Mule::Window::Create();

	mGraphicsDevice = Mule::GraphicsDevice::Create(Mule::GraphicsAPI::Vulkan, mWindow);

	Mule::ImGuiRenderer::Init(mGraphicsDevice);

	mApplicationData = Mule::Ref<ApplicationData>::Make();
	mApplicationData->ImGuiContext = Mule::ImGuiRenderer::CreateContext();
	mApplicationData->GraphicsDevice = mGraphicsDevice;
	

	Mule::FramebufferDesc framebufferDesc{};
	framebufferDesc.Name = "UI Framebuffer";
	framebufferDesc.BuildForSwapChain = true;
	framebufferDesc.Device = mGraphicsDevice;
	framebufferDesc.Width = 800;
	framebufferDesc.Height = 600;
	framebufferDesc.Attachments = {
		Mule::FrameBufferAttachment("Color Attachment", Mule::TextureFormat::BGRA8_UNORM, Mule::Samples::MSAA_1, Mule::ClearValue(glm::vec4(0.f, 0.f, 0.f, 0.f)))
	};
	framebufferDesc.DepthAttachment = {
		Mule::FrameBufferAttachment("Depth Attachment", Mule::TextureFormat::Depth24Stencil8, Mule::Samples::MSAA_1, Mule::ClearValue(glm::vec4(1.f, 0.f, 0.f, 0.f)))
	};
	framebufferDesc.SubPassDescription = {
		Mule::SubPassDescription({0}, true)
	};

	// A bit ugly for an api bit this will work just fine for now
	framebufferDesc.SwapChainFrameIndex = 0;

	mFramebuffers[0] = Mule::FrameBuffer::Create(framebufferDesc);

	framebufferDesc.SwapChainFrameIndex = 1;
	mFramebuffers[1] = Mule::FrameBuffer::Create(framebufferDesc);

	mCommandBuffers[0] = Mule::CommandBuffer::Create(mGraphicsDevice);
	mCommandBuffers[1] = Mule::CommandBuffer::Create(mGraphicsDevice);

	Mule::ImGuiRenderer::CreateFont("C:/Dev/Mule/Resources/Fonts/Roboto/Roboto-Medium.ttf");
}

void Application::Run()
{
	ImGuiContext* context = mApplicationData->ImGuiContext;

	//while (mAppIsRunning)
	{
		mWindow->PollEvents();

		mGraphicsDevice->BeginFrame();

		if (!mLayers.empty())
		{
			auto layer = mLayers.top();
			layer->OnUpdate(0.f);

			int frameIndex = mGraphicsDevice->GetFrameIndex();

			Mule::Ref<Mule::FrameBuffer> framebuffer = mFramebuffers[frameIndex];
			Mule::Ref<Mule::CommandBuffer> commandBuffer = mCommandBuffers[frameIndex];

			commandBuffer->Begin();
			framebuffer->Begin(commandBuffer);

			ImGui::SetCurrentContext(context);
			Mule::ImGuiRenderer::BeginFrame();
			layer->OnUIRender();
			Mule::ImGuiRenderer::EndFrame();
			Mule::ImGuiRenderer::Render(commandBuffer);

			framebuffer->End(commandBuffer);
			commandBuffer->End();

			mGraphicsDevice->SubmitCommands(commandBuffer, {}, true);
		}

		mGraphicsDevice->EndFrame();
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

