#include "mulepch.h"
#include "Mule/Renderer/Camera.h"

#include <glm/gtx/transform.hpp>

namespace Mule
{
	Camera::Camera(const glm::vec3& position, const glm::vec3& direction, float aspectRatio, float fovRadians, float pitchRads, float yawRads, float nearPlane, float farPlane, const glm::vec3& up)
		:
		mPosition(position),
		mDirection(direction),
		mAspectRatio(aspectRatio),
		mFOVRadians(fovRadians),
		mPitchRads(pitchRads),
		mYawRads(yawRads),
		mNearPlane(nearPlane),
		mFarPlane(farPlane),
		mUp(up)
	{
		Update();
	}

	void Camera::LookInDirection(const glm::vec3& direction)
	{
		mDirection = direction;
		Update();
	}
	
	void Camera::SetFOVRads(float fov)
	{
		mFOVRadians = fov;
		Update();
	}
	
	void Camera::SetNearPlane(float nearPlane)
	{
		mNearPlane = nearPlane;
		Update();
	}
	
	void Camera::SetFarPlane(float farPlane)
	{
		mFarPlane = farPlane;
		Update();
	}
	
	void Camera::SetAspectRatio(float aspectRatio)
	{
		mAspectRatio = aspectRatio;
		Update();
	}

	void Camera::SetPitchRads(float pitch)
	{
		mPitchRads = pitch;
		// TODO: Rotate mDirection
		Update();
	}

	void Camera::SetYawRads(float yaw)
	{
		mYawRads = yaw;
		// TODO: Rotate mDirection
		Update();
	}
	
	void Camera::Update()
	{
		mView = glm::lookAt(mPosition, mPosition + mDirection, mUp);
		mPerspective = glm::perspective(mFOVRadians, mAspectRatio, mNearPlane, mFarPlane);
		// TODO: temporary fix for vulkan, may want to add a creation flag for up direction
		mPerspective[1][1] *= -1;
	}
}
