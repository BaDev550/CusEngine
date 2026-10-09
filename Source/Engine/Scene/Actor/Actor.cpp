#include "Actor.h"

#include <Runtime/Reflection/TypeRegistry.h>

namespace CusEngine {
	void CActor::SetRotation(const glm::vec3& rot) {
		Logger::Info("CActor", "Rotation changed");
	}
}