#include "SandboxLayer.h"

void SandBoxLayer::OnAttach()
{
	Mule::Shader::Compile("../Resource/Shaders/source/shader.vert", "../Resource/Shaders/spir-v/vert.spv");
	Mule::Shader::Compile("../Resource/Shaders/source/shader.frag", "../Resource/Shaders/spir-v/frag.spv");

	mUniformBuffer = Mule::UniformBuffer::Create({ sizeof(UniformBufferData), 0, Mule::ShaderStage::VERTEX });

	Mule::ShaderDescription shaderDesc(
		{
			Mule::ShaderAttachment{ Mule::ShaderStage::VERTEX, "../Resource/Shaders/spir-v/vert.spv", sizeof(VertexPushConstant) },
			Mule::ShaderAttachment{ Mule::ShaderStage::FRAGMENT, "../Resource/Shaders/spir-v/frag.spv", 0 }//sizeof(FragmentPushConstant) }
		},
		{
			mUniformBuffer
		},
		Mule::VertexLayout::Default()
	);

	mShader = Mule::Shader::Create(shaderDesc);

	Mule::Camera camera = Mule::Camera({ 50, 50, 50 }, glm::normalize(glm::vec3{ -1, -1, -1 }), 1.f, 90.f);

	mModel = Mule::MeshLoader::LoadMesh("../Resource/Models/Obj/Spider/Spider.obj");

	mUniformData.model = glm::identity<glm::mat4>();
	mUniformData.view = camera.GetView();
	mUniformData.proj = camera.GetPerspective();

	mVertexPushConstantData.model = glm::identity<glm::mat4>();
	mVertexPushConstantData.view = camera.GetView();
	mVertexPushConstantData.proj = camera.GetPerspective();

}

void SandBoxLayer::OnUpdate()
{
	mUniformBuffer->SetBufferData(&mUniformData);

	mUniformBuffer->Bind(mShader);
	mShader->Bind();
	mShader->BindPushConstant(Mule::ShaderStage::VERTEX, &mVertexPushConstantData);

	for (auto& mesh : mModel->GetMeshes())
	{
		mesh->Draw();
	}
}

void SandBoxLayer::OnDetach()
{
}
