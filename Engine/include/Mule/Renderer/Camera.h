#pragma once

#include <glm/glm.hpp>

namespace Mule
{
	class Camera
	{
	public:
		Camera(const glm::vec3& position, const glm::vec3& direction, float aspectRatio, float fovRadians, float pitchRads = 0.f, float yawRads = 0.f, float nearPlane = 1.f, float farPlane = 1000.f, const glm::vec3& up = glm::vec3(0.f, 1.f, 0.f));

		void LookInDirection(const glm::vec3& direction);

		const glm::vec3& GetPosition() const { return mPosition; }
		void SetPosition(const glm::vec3& position) { mPosition = position; }

		float GetFOVRads() const { return mFOVRadians; }
		void SetFOVRads(float fov);

		float GetNearPlane() const { return mNearPlane; }
		void SetNearPlane(float nearPlane);

		float GetFarPlane() const { return mFarPlane; }
		void SetFarPlane(float farPlane);

		float GetAspectRatio() const { return mAspectRatio; }
		void SetAspectRatio(float aspectRatio);

		const glm::mat4& GetPerspective() { return mPerspective; }
		const glm::mat4& GetView() { return mView; }

		float GetPitchRads() const { return mPitchRads; }
		void SetPitchRads(float pitch);

		float GetYawRads() const { return mYawRads; }
		void SetYawRads(float yaw);


	private:
		void Update();

		glm::vec3 mPosition;
		glm::vec3 mUp;
		glm::vec3 mDirection;

		glm::mat4 mPerspective;
		glm::mat4 mView;

		float mNearPlane, mFarPlane, mFOVRadians, mAspectRatio, mPitchRads, mYawRads;
	};
}
