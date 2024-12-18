#pragma once

#include "Asset/Asset.h"
#include "Ref.h"

#include <glm.hpp>

#include "FrameBuffer.h"
#include "RenderPass.h"
#include "Shader.h"
#include "UniformBuffer.h"
#include "ECS/Scene.h"
#include "ShaderConstant.h"

#include "ECS/Components.h"

namespace Mule
{
	class SceneRenderer
	{
	public:
		static Ref<SceneRenderer> Create(Ref<GraphicsDevice> device, RenderContext context);

		void Render(Ref<Scene> scene);
		ImTextureID GetFBImage(uint32_t index);
		void Resize(uint32_t width, uint32_t height);

		~SceneRenderer();
	private:
		SceneRenderer(Ref<GraphicsDevice> device, RenderContext context);

		Ref<GraphicsDevice> mDevice;
		Ref<FrameBuffer> mFramebuffer;
		Ref<RenderPass> mRenderPass;
		Ref<Shader> mDefaultShader;
		Ref<UniformBuffer> mCameraUBO;
		Ref<ShaderConstant> mShaderConstant;

		struct alignas(16) CameraData
		{
			glm::mat4 VP;
			glm::vec3 Position;
			glm::vec3 ViewDir;
		};
	};
}