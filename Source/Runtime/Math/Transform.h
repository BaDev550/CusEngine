#pragma once

#include <Runtime/Definitions/Types.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct Transform {
	glm::vec3 position = glm::vec3(1.0f);
	glm::quat rotation = glm::quat(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec3 scale = glm::vec3(1.0f);

	Transform operator*(const Transform& other) const {
		return {
			other.position * position,
			other.rotation * rotation,
			other.scale * scale
		};
	}

	Transform operator+(const Transform& other) const {
		return {
			other.position + position,
			other.rotation + rotation,
			other.scale + scale
		};
	}

	Transform(const glm::vec3& pos, const glm::quat& rot, const glm::vec3& scl) : position(pos), rotation(rot), scale(scl) {}
	Transform(const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& scl) : position(pos), rotation(rot), scale(scl) {}
	Transform() : position({ 0.0f }), rotation({ 0.0f, 0.0f, 0.0f, 1.0f }), scale({ 1.0f }) {}
private:
	glm::vec3 rotationEuler;
};