#pragma once

#include "ILayer.h"

#include "Mule/Mule.h"

class SandBoxLayer : public ILayer
{
public:
	virtual void OnAttach() override;
	virtual void OnUpdate() override;
	virtual void OnDetach() override;

private:
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

	UniformBufferData mUniformData;
	VertexPushConstant mVertexPushConstantData;

	Mule::Ref<Mule::UniformBuffer> mUniformBuffer;
	Mule::Ref<Mule::Shader> mShader;
	Mule::Ref<Mule::MeshCollection> mModel;

};