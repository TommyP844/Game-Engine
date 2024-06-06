#include "Graphics/SceneRenderer.h"

#include "Graphics/Mesh.h"
#include "Graphics/Material.h"

#include "Asset/AssetManager.h"

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

namespace Mule
{

	Ref<SceneRenderer> SceneRenderer::Create(Ref<GraphicsDevice> device, RenderContext context)
	{
		return Ref<SceneRenderer>(new SceneRenderer(device, context));
	}

	SceneRenderer::~SceneRenderer()
	{
	}

	SceneRenderer::SceneRenderer(Ref<GraphicsDevice> device, RenderContext context)
		:
		mDevice(device)
	{

		UniformBufferDescription UBODesc;
		UBODesc.AccessMode = BufferAccess::Write;
		UBODesc.IsBufferDynamic = true;
		UBODesc.Name = "Light UBO";
		UBODesc.Size = sizeof(DirectionalLight) + sizeof(Light) * MAX_LIGHTS;

		mLightUBO = Mule::UniformBuffer::Create(mDevice, UBODesc);

		UBODesc.Name = "Camera UBO";
		UBODesc.Size = sizeof(CameraData);

		mCameraUBO = Mule::UniformBuffer::Create(mDevice, UBODesc);

		AssetManager& manager = AssetManager::Get();
		mRenderPass = manager.GetAsset<RenderPass>(Mule::DefaultRenderPassHandle);
		mDefaultShader = manager.GetAsset<Shader>(Mule::DefaultShaderHandle);

		FrameBufferDescription FBDesc;
		FBDesc.Name = "Scene Framebuffer";
		FBDesc.Width = 800;
		FBDesc.Height = 600;
		FBDesc.BuildForSwapChain = false;
		FBDesc.Attachments = {
			FrameBufferAttachment(TextureFormat::RGBA8, (TextureFlags)(TextureFlags::RenderTarget | TextureFlags::ShaderInput), {0.f, 0.f, 0.f, 0.f}),
			FrameBufferAttachment(TextureFormat::Depth32F, TextureFlags::RenderTarget, {1.f, 0.f, 0.f, 0.f})
		};
		FBDesc.RenderPass = mRenderPass;
		FBDesc.SampleCount = Samples::SampleCount_1;
		FBDesc.Context = context;

		mFramebuffer = FrameBuffer::Create(mDevice, FBDesc);
	}

	void SceneRenderer::Render(Ref<Scene> scene)
	{
		AssetManager& manager = AssetManager::Get();
		auto& registry = scene->GetRegistry();

		// populate lights
		auto cameraView = registry.view<CameraComponent>();
		if (cameraView.size() == 0) return;
		for (auto entityId : cameraView)
		{
			Entity e(entityId, scene);
			auto& cameraComponent = e.GetComponent<CameraComponent>();
			if (cameraComponent.Primary)
			{
				Camera camera = cameraComponent.Camera;
				CameraData cameraData;
				
				mCameraUBO->SetData(mDevice->GetMainThreadContext(), &cameraData, sizeof(CameraData));
				break;
			}
		}


		// upload camera data


		mRenderPass->Begin(mFramebuffer);

		auto shader = manager.GetAsset<Shader>(DefaultShaderHandle);

		shader->Bind();
		shader->SetUniformBuffer(Mule::ShaderStage::Vertex, "CameraBuffer", mCameraUBO);
		//shader->SetUniformBuffer(Mule::ShaderStage::Fragment, "LightBuffer", mLightUBO);
		//shader->BindResources();

		auto meshView = registry.view<MeshComponent>();
		for (auto entityId : meshView)
		{
			Entity e(entityId, scene);
			auto& meshComponent = e.GetComponent<MeshComponent>();
			auto& transform = e.Transform();

			glm::mat4 trs = transform.TRS();
			// upload trs

			//shader->SetShaderVar<float>("PushConstant", Mule::ShaderStage::Vertex, &trs[0][0]);

			auto mesh = manager.GetAsset<Mesh>(meshComponent.MeshHandle);
			if (mesh)
			{
				mesh->Bind();
				mesh->Draw();
			}
		}

		mRenderPass->End();
	}

	ImTextureID SceneRenderer::GetFBImage(uint32_t index)
	{
		uint32_t imageIndex = std::min(mFramebuffer->GetNumAttachments(), index);
		return mFramebuffer->GetAttachmentViewImGui(imageIndex);
	}

	void SceneRenderer::Resize(uint32_t width, uint32_t height)
	{
		mFramebuffer->Resize(width, height);
	}
}
