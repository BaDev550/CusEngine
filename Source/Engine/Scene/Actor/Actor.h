#pragma once

#include <Engine/Core/Core.h>
#include <Engine/Reflection/Object.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace CusEngine {
	class CActor : public CObject {
	public:
		CActor() = default;
		virtual ~CActor() = default;

		__forceinline const glm::vec3& GetPosition() const { return _position; }
		__forceinline const glm::quat& GetRotation() const { return _rotation; }
		__forceinline const glm::vec3& GetScale() const { return _scale; }
	private:
		glm::vec3 _position;
		glm::quat _rotation;
		glm::vec3 _rotationEuler;
		glm::vec3 _scale;
	};
}