#include "Actor.h"

#include <Runtime/Definitions/Logger.h>

namespace Tourqe::Engine {
	void TActor::SetRotation(const glm::vec3& rot) {
		Logger::Info("CActor", "Rotation changed");
	}
}