#pragma once

#include <Engine/World/Components/BaseComponent.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Tourqe::Engine {
	TCLASS();
	class TransformComponent : public BaseComponent {
		GENERATE_CLASS(TransformComponent);
	public:
		TPROP();
		glm::vec3 position = glm::vec3(1.0f);

		TPROP();
		glm::quat rotation = glm::quat(0.0f, 0.0f, 0.0f, 1.0f);

		TPROP();
		glm::vec3 scale = glm::vec3(1.0f);

		TransformComponent operator*(const TransformComponent& other) const {
			return {
				other.position * position,
				other.rotation * rotation,
				other.scale * scale
			};
		}

		TransformComponent operator+(const TransformComponent& other) const {
			return {
				other.position + position,
				other.rotation + rotation,
				other.scale + scale
			};
		}

		TransformComponent(const glm::vec3& pos, const glm::quat& rot, const glm::vec3& scl) : position(pos), rotation(rot), scale(scl) {}
		TransformComponent(const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& scl) : position(pos), rotation(rot), scale(scl) {}
		TransformComponent() : position({ 0.0f }), rotation({ 0.0f, 0.0f, 0.0f, 1.0f }), scale({ 1.0f }) {}
	private:
		TPROP();
		glm::vec3 rotationEuler;
	};
}