#pragma once

#include <Engine/Core/Core.h>
#include <Engine/Core/Object.h>
#include <Runtime/Reflection/ReflectionMacros.h>

#include <glm/gtc/quaternion.hpp>
#include <glm/glm.hpp>

namespace CusEngine {
	CCLASS()
	class CActor : public CObject {
		GENERATE_CLASS(CActor)
	public:
		CActor() = default;
		virtual ~CActor() = default;

		void SetRotation(const glm::vec3& rot);

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