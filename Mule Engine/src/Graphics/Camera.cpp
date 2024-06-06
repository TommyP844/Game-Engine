#include "Graphics/Camera.h"

#include <gtx/transform.hpp>

namespace Mule
{
	Camera::Camera()
		:
		mPosition(glm::vec3(0.f, 0.f, 0.f)),
		mForwardDir(glm::vec3(0.f, 0.f, -1.f)),
		mWorldUp(glm::vec3(0.f, 1.f, 0.f)),
		mAspectRatio(1.f),
		mFOVDegrees(90.f),
		mNearPlane(1.f),
		mFarPlane(1000.f)
	{
		mRightDir = glm::cross(mForwardDir, mWorldUp);
		mUpDir = glm::cross(mForwardDir, mRightDir);
		CalcViewMatrix();
		CalcPerspectiveMatrix();
		CalcVPMatrix();
	}

	Camera::Camera(const glm::vec3& position,
		const glm::vec3& viewDir,
		const glm::vec3& worldUp,
		float aspectRatio,
		float fovDegrees,
		float nearPlane,
		float farPlane)
		:
		mPosition(position),
		mForwardDir(viewDir),
		mWorldUp(worldUp),
		mAspectRatio(aspectRatio),
		mFOVDegrees(fovDegrees),
		mNearPlane(nearPlane),
		mFarPlane(farPlane)
	{
		mRightDir = glm::cross(mForwardDir, mWorldUp);
		mUpDir = glm::cross(mForwardDir, mRightDir);
		CalcViewMatrix();
		CalcPerspectiveMatrix();
		CalcVPMatrix();
	}

	void Camera::PanTilt(float dx, float dy)
	{
		mPitchDegrees += dx;
		mYawDegrees += dy;

		CalcViewDir();
	}

	void Camera::SetPosition(const glm::vec3& position)
	{
		mPosition = position;
		CalcViewMatrix();
		CalcVPMatrix();
	}

	void Camera::SetViewDir(const glm::vec3& viewDir)
	{
		mForwardDir = glm::normalize(viewDir);
		mRightDir = glm::cross(mForwardDir, mWorldUp);
		mUpDir = glm::cross(mForwardDir, mRightDir);
		mYawDegrees = glm::degrees(atan2(mForwardDir.z, mForwardDir.x));
		mPitchDegrees = glm::degrees(asin(mForwardDir.y));
		CalcViewMatrix();
		CalcVPMatrix();
	}

	void Camera::SetWorldUp(const glm::vec3& worldUp)
	{
		mWorldUp = worldUp;
		mRightDir = glm::cross(mForwardDir, mWorldUp);
		mUpDir = glm::cross(mForwardDir, mRightDir);
		CalcViewMatrix();
		CalcVPMatrix();
	}

	void Camera::SetAspectRatio(float aspectRatio)
	{
		mAspectRatio = aspectRatio;
		CalcPerspectiveMatrix();
		CalcVPMatrix();
	}

	void Camera::SetNearPlane(float nearPlane)
	{
		mNearPlane = nearPlane;
		CalcPerspectiveMatrix();
		CalcVPMatrix();
	}

	void Camera::SetFarPlane(float farPlane)
	{
		mFarPlane = farPlane;
		CalcPerspectiveMatrix();
		CalcVPMatrix();
	}

	void Camera::SetFOVDegrees(float degrees)
	{
		mFOVDegrees = degrees;
		CalcPerspectiveMatrix();
		CalcVPMatrix();
	}

	void Camera::SetYawDegrees(float degrees)
	{
		mYawDegrees = degrees;
		CalcViewDir();
	}

	void Camera::SetPitchDegrees(float degrees)
	{
		mPitchDegrees = degrees;
		CalcViewDir();
	}

	void Camera::CalcViewMatrix()
	{
		mViewMatrix = glm::lookAt(mPosition, mPosition + mForwardDir, mWorldUp);
	}

	void Camera::CalcPerspectiveMatrix()
	{
		mPerspectiveMatrix = glm::perspective(mFOVDegrees, mAspectRatio, mNearPlane, mFarPlane);
	}

	void Camera::CalcVPMatrix()
	{
		mVPMatrix = mPerspectiveMatrix * mViewMatrix;
	}

	void Camera::CalcViewDir()
	{
		float yawRad = glm::radians(mYawDegrees);
		float pitchRad = glm::radians(mPitchDegrees);

		mForwardDir.x = glm::cos(pitchRad) * glm::cos(yawRad);
		mForwardDir.y = glm::sin(pitchRad);
		mForwardDir.z = glm::cos(pitchRad) * glm::sin(yawRad);

		mForwardDir = glm::normalize(mForwardDir);

		CalcViewMatrix();
		CalcVPMatrix();
	}

}