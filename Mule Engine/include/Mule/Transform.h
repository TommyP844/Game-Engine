#pragma once

#include "Ref.h"
#include <glm.hpp>
#include <gtx/quaternion.hpp>

namespace Mule
{
	class Transform
	{
	public:
		Transform(
			const glm::vec3& translation = glm::vec3(0.f),
			const glm::quat& orientation = glm::quat(0.f, 0.f, 0.f, 1.f),
			const glm::vec3& scale = glm::vec3(1.f))
			:
			Translation(translation),
			Orientation(orientation),
			Scale(scale)
		{}

		~Transform() {}

		glm::vec3 Translation;
		glm::quat Orientation;
		glm::vec3 Rotation;
		glm::vec3 Scale;

		glm::mat4 TRS()
		{
			glm::mat4 translation = glm::translate(glm::identity<glm::mat4>(), Translation);
			glm::mat4 orientation = glm::toMat4(Orientation);
			glm::mat4 scale = glm::scale(glm::identity<glm::mat4>(), Scale);

			return translation * orientation * scale;
		}

		glm::mat4 TR()
		{
			glm::mat4 translation = glm::translate(glm::mat4(1.f), Translation);
			glm::mat4 orientation = glm::toMat4(Orientation);

			return translation * orientation;
		}

		glm::mat4 RT()
		{
			glm::mat4 translation = glm::translate(glm::mat4(1.f), Translation);
			glm::mat4 orientation = glm::toMat4(Orientation);

			return orientation * translation;
		}

		glm::mat4 TranslationMatrix()
		{
			glm::mat4 translation = glm::translate(glm::mat4(1.f), Translation);

			return translation;
		}

		glm::mat4 OrientationMatrix()
		{
			glm::mat4 orientation = glm::toMat4(Orientation);

			return orientation;
		}

		glm::mat4 ScaleMatrix()
		{
			glm::mat4 scale = glm::scale(glm::mat4(1.f), Scale);

			return scale;
		}
	};
}