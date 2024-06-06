#pragma once

#include "Ref.h"
#include "Guid.h"
#include "Entity.h"
#include "Transform.h"
#include "Graphics/Camera.h"

#include <string>

namespace Mule
{

	struct MetaComponent
	{
		MetaComponent() = default;
		MetaComponent(const MetaComponent& other) = default;

		std::string Tag;
		Transform Transform;
		Guid Guid;
		Entity Parent;
		std::set<Entity> Children;
	};

	// Used to filter root entities
	struct RootComponent 
	{
		RootComponent() = default;
		const bool IsRoot = true;
	};

	struct ScriptComponent
	{
		ScriptComponent() = default;
		ScriptComponent(const ScriptComponent&) = default;

		const bool TODO = true;
		//Ref<ScriptableClass> Class;
	};

	struct CameraComponent
	{
		CameraComponent(const CameraComponent& other) = default;
		CameraComponent() = default;

		bool Primary;
		Camera Camera;
	};

	struct MeshComponent
	{
		MeshComponent()
			:
			mGuid(Asset::GenerateHandle())
		{}
		MeshComponent(const MeshComponent& other) = default;
		AssetHandle ShaderHandle;
		AssetHandle MeshHandle;
		AssetHandle MaterialHandle;
		bool Visible;
		bool CastsShadows;
		bool ReceiveShadows;

		AssetHandle GetGuid() const { return mGuid; }

	private:
		AssetHandle mGuid;
	};

	struct MeshCollectionComponent
	{
		std::vector<MeshComponent> Meshes;
	};

	struct PointLightComponent
	{
		PointLightComponent() = default;
		PointLightComponent(const PointLightComponent&) = default;

		glm::vec3 Color = glm::vec3(1.f);
		float Intensity = 1.f;
	};

	struct SpotLightComponent
	{
		SpotLightComponent() = default;
		SpotLightComponent(const SpotLightComponent&) = default;

		glm::vec3 Color = glm::vec3(1.f);
		float Intensity = 1.f;
		float Angle;
	};

	struct DirectionalLightComponent
	{
		DirectionalLightComponent() = default;
		DirectionalLightComponent(const DirectionalLightComponent&) = default;

		glm::vec3 Color = glm::vec3(1.f);
		float Intensity = 1.f;
	};
}