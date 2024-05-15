#pragma once

#include "Ref.h"
#include "Guid.h"
#include "Entity.h"
#include "Transform.h"
#include "Graphics/Camera.h"

#include <string>

namespace Mule
{
	struct IComponent
	{
		virtual ~IComponent(){}
		bool Visible = true;
		bool Active = true;
		const Guid& GetRuntimeGuid() const { return mGuid; }
	protected:
		IComponent() = default;
	private:
		Guid mGuid;
	};

	struct TagComponent
	{
		TagComponent() = default;
		TagComponent(const TagComponent& other) = default;
		TagComponent(const std::string& tag)
		{
			Tag = tag;
		}

		std::string Tag;

		virtual void Init() = delete;
		virtual void Destroy() = delete;
	};

	struct GuidComponent
	{
		GuidComponent() = default;
		GuidComponent(const GuidComponent& other) = default;

		Guid Guid;

		virtual void Init() = delete;
		virtual void Destroy() = delete;
	};

	struct RelationshipComponent
	{
		RelationshipComponent() = default;
		RelationshipComponent(const RelationshipComponent& other) = default;

		Entity Parent;
		std::vector<Entity> Children;

		virtual void Init() = delete;
		virtual void Destroy() = delete;
	};

	struct TransformComponent
	{
		TransformComponent() = default;
		TransformComponent(const TransformComponent& other) = default;

		Transform Transform;
	};

	struct ScriptComponent
	{
		ScriptComponent() = default;
		ScriptComponent(const ScriptComponent&) = default;

		//Ref<ScriptableClass> Class;
	};

	struct CameraComponent
	{
		CameraComponent(const CameraComponent& other) = default;
		CameraComponent() = default;

		bool Primary;
		Camera Camera;
	};

	struct ModelComponentMesh
	{
		AssetHandle MeshHandle;
		AssetHandle MaterialHandle;
		bool Visible;
		bool CastsShadows;
		bool ReceiveShadows;
	};

	struct ModelComponent
	{
		ModelComponent() = default;
		ModelComponent(const ModelComponent& other) = default;

		AssetHandle ModelHandle;
		std::vector<ModelComponentMesh> MeshData;
		bool Visible;
		bool CastsShadows;
		bool ReceiveShadows;
	};

	struct PointLightComponent : IComponent
	{
		PointLightComponent() = default;
		PointLightComponent(const PointLightComponent&) = default;

		glm::vec3 Color = glm::vec3(1.f);
		float Intensity = 1.f;
		float AttenuationConstant = 1.f;
		float AttenuationLinear = 0.f;
		float AttenuationQuadratic = 0.f;
	};

	struct SpotLightComponent : IComponent
	{
		SpotLightComponent() = default;
		SpotLightComponent(const SpotLightComponent&) = default;

		glm::vec3 Color = glm::vec3(1.f);
		float Intensity = 1.f;
		float AttenuationConstant = 1.f;
		float AttenuationLinear = 0.f;
		float AttenuationQuadratic = 0.f;
		float Theta;
	};

	struct DirectionalLightComponent : IComponent
	{
		DirectionalLightComponent() = default;
		DirectionalLightComponent(const DirectionalLightComponent&) = default;

		glm::vec3 Color = glm::vec3(1.f);
		float Intensity = 1.f;
	};
}