

#include "Mule/Mule.h"

#include <iostream>

#include <vulkan/vulkan.h>

int main(int argc, char** argv)
{
	Mule::Window::Init();
	Mule::Ref<Mule::Window> window = Mule::Window::Create(800, 600, "Mule");
	//Mule::Internal::ThreadPool::Init();
	Mule::RenderAPI::Init(Mule::RenderAPI::API::VULKAN, window->GetWindowPtr());

	Mule::Shader::Compile("../Resource/Shaders/source/shader.vert", "../Resource/Shaders/spir-v/vert.spv");
	Mule::Shader::Compile("../Resource/Shaders/source/shader.frag", "../Resource/Shaders/spir-v/frag.spv");

	struct UniformBufferData
	{
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};

	struct VertexPushConstant
	{
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};

	Mule::Ref<Mule::UniformBuffer> uniformBuffer = Mule::UniformBuffer::Create({ sizeof(UniformBufferData), 0, Mule::ShaderStage::VERTEX });
	
	Mule::ShaderDescription shaderDesc(
		{
			Mule::ShaderAttachment{ Mule::ShaderStage::VERTEX, "../Resource/Shaders/spir-v/vert.spv", sizeof(VertexPushConstant) },
			Mule::ShaderAttachment{ Mule::ShaderStage::FRAGMENT, "../Resource/Shaders/spir-v/frag.spv", 0 }//sizeof(FragmentPushConstant) }
		},
		{ 
			uniformBuffer 
		},
		Mule::VertexLayout::Default()
	);

	Mule::Ref<Mule::Shader> shader = Mule::Shader::Create(shaderDesc);

	Mule::Camera camera = Mule::Camera({ 50, 50, 50 }, glm::normalize(glm::vec3{-1, -1, -1}), 1.f, 90.f);

	Mule::Ref<Mule::MeshCollection> model = Mule::MeshLoader::LoadMesh("../Resource/Models/Obj/Spider/Spider.obj");

	UniformBufferData uniformBufferData{};

	uniformBufferData.model = glm::identity<glm::mat4>();
	uniformBufferData.view = camera.GetView();
	uniformBufferData.proj = camera.GetPerspective();

	VertexPushConstant vertexData;

	vertexData.model = glm::identity<glm::mat4>();
	vertexData.view = camera.GetView();
	vertexData.proj = camera.GetPerspective();


	while (true) 
	{
		uniformBuffer->SetBufferData(&uniformBufferData);
		window->PollEvents();
		Mule::RenderAPI::BeginFrame();

		uniformBuffer->Bind(shader);
		shader->Bind();
		shader->BindPushConstant(Mule::ShaderStage::VERTEX, &vertexData);

		for (auto& mesh : model->GetMeshes())
		{
			mesh->Draw();
		}

		Mule::RenderAPI::EndFrame();
		Mule::RenderAPI::SwapBuffers();
	}
	
	
	Mule::RenderAPI::Shutdown();
	//Mule::Internal::ThreadPool::Shutdown();
	Mule::Window::Shutdown();

	return 0;
}