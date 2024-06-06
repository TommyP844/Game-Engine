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
			const glm::vec3& rotation = glm::vec3(0.f, 0.f, 0.f),
			const glm::vec3& scale = glm::vec3(1.f))
			:
			Translation(translation),
			Rotation(rotation),
			Scale(scale)
		{}

		~Transform() {}

		glm::vec3 Translation;
		glm::vec3 Rotation;
		glm::vec3 Scale;

		glm::quat GetOrientation()
		{
			glm::quat q = glm::angleAxis(Rotation.x, glm::vec3(1.f, 0.f, 0.f))
				* glm::angleAxis(Rotation.y, glm::vec3(0.f, 1.f, 0.f))
				* glm::angleAxis(Rotation.z, glm::vec3(0.f, 0.f, 1.f));
			return q;
		}

		glm::mat4 TRS()
		{
			glm::mat4 translation = glm::translate(glm::identity<glm::mat4>(), Translation);
			glm::mat4 orientation = glm::toMat4(GetOrientation());
			glm::mat4 scale = glm::scale(glm::identity<glm::mat4>(), Scale);

			return translation * orientation * scale;
		}

		glm::mat4 TR()
		{
			glm::mat4 translation = glm::translate(glm::mat4(1.f), Translation);
			glm::mat4 orientation = glm::toMat4(GetOrientation());

			return translation * orientation;
		}

		glm::mat4 RT()
		{
			glm::mat4 translation = glm::translate(glm::mat4(1.f), Translation);
			glm::mat4 orientation = glm::toMat4(GetOrientation());

			return orientation * translation;
		}

		glm::mat4 TranslationMatrix()
		{
			glm::mat4 translation = glm::translate(glm::mat4(1.f), Translation);

			return translation;
		}

		glm::mat4 OrientationMatrix()
		{
			glm::mat4 orientation = glm::toMat4(GetOrientation());

			return orientation;
		}

		glm::mat4 ScaleMatrix()
		{
			glm::mat4 scale = glm::scale(glm::mat4(1.f), Scale);

			return scale;
		}
	};
}