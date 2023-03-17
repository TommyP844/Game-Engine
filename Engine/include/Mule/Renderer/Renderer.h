#pragma once

#include "../Ref.h"
#include "../Window/Window.h"
#include "Mule/Renderer/Framebuffer.h"
#include "Mule/Renderer/Material.h"
#include "Mule/Renderer/Shader.h"
#include "Mule/Renderer/Texture.h"
#include "Mule/Renderer/MeshCollection.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Mule
{
	namespace Renderer
	{
		enum CommandType
		{
			Func,
			Draw,
			Compute
		};

		struct DrawData
		{
			Ref<MeshCollection> Mesh;
			Ref<Shader> Shader;
			Ref<Material> Material;
			std::function<void()> BindFunc;
			int ModelIndex;
			CommandType Type;
			glm::mat4 Model;
		};

		struct RenderData
		{
			Ref<Window> Window;

			std::vector<glm::mat4> ModelTransforms;
			glm::mat4 View;
			glm::mat4 Proj;
			std::vector<DrawData> DrawData;
			Ref<FrameBuffer> Framebuffer;
		};

		bool Init(Ref<Window> window);
		void Shutdown();

		void NewImGuiFrame();
		void EndImGuiFrame();

		void BeginFrame(const glm::mat4& view, const glm::mat4& proj, Ref<FrameBuffer> framebuffer = nullptr);

		// void SetView(const glm::mat4& view, const glm::mat4& proj);

		void Submit(Ref<MeshCollection> mesh, Ref<Shader> shader, Ref<Material> material, const glm::vec3& worldPos, const glm::mat4& model, std::function<void()> func = nullptr);

		void Submit(Ref<Shader> shader, std::function<void()> func);


		void Submit(std::function<void()> func);

		void Render();

		void EndFrame();
	}
}