#pragma once

#include <glm.hpp>

namespace Mule
{
	class Camera
	{
	public:
		Camera();
		Camera(const glm::vec3& position,
			const glm::vec3& viewDir = glm::vec3(0.f, 0.f, -1.f),
			const glm::vec3& worldUp = glm::vec3(0.f, 1.f, 0.f),
			float aspectRatio = 1.f,
			float fovDegrees = 90.f,
			float nearPlane = 1.f,
			float farPlane = 1000.f);

		void PanTilt(float dx, float dy);

		void SetPosition(const glm::vec3& position);
		void SetViewDir(const glm::vec3& viewDir);
		void SetWorldUp(const glm::vec3& worldUp);
		void SetAspectRatio(float aspectRatio);
		void SetNearPlane(float nearPlane);
		void SetFarPlane(float farPlane);
		void SetFOVDegrees(float degrees);
		void SetYawDegrees(float degrees);
		void SetPitchDegrees(float degrees);

		const glm::vec3& GetPosition() const { return mPosition; }
		const glm::vec3& GetForwardDir() const { return mForwardDir; }
		const glm::vec3& GetRightDir() const { return mRightDir; }
		const glm::vec3& GetWorldUp() const { return mWorldUp; }
		const glm::vec3& GetUpDir() const { return mUpDir; }

		float GetAspectRatio() const { return mAspectRatio; }
		float GetNearPlane() const { return mNearPlane; }
		float GetFarPlane() const { return mFarPlane; }
		float GetFOVDegrees() const { return mFOVDegrees; }
		float GetYawDegrees() const { return mYawDegrees; }
		float GetPitchDegrees() const { return mPitchDegrees; }

		const glm::mat4& GetPerspectiveMatrix() const { return mPerspectiveMatrix; }
		const glm::mat4& GetViewMatrix() const { return mViewMatrix; }
		const glm::mat4& GetViewPerspectiveMatric() const { return mVPMatrix; }

	private:
		void CalcViewMatrix();
		void CalcPerspectiveMatrix();
		void CalcVPMatrix();
		void CalcViewDir();

		glm::vec3 mPosition;
		glm::vec3 mForwardDir;
		glm::vec3 mRightDir;
		glm::vec3 mWorldUp;
		glm::vec3 mUpDir;

		float mAspectRatio;
		float mFOVDegrees;
		float mNearPlane;
		float mFarPlane;
		float mYawDegrees;
		float mPitchDegrees;

		glm::mat4 mPerspectiveMatrix;
		glm::mat4 mViewMatrix;
		glm::mat4 mVPMatrix;
	};
}
