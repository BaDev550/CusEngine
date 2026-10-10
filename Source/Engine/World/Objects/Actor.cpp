#include "Actor.h"

#include <Runtime/Definitions/Logger.h>

namespace Tourqe::Engine {
	const glm::vec3& TActor::GetWorldPosition() const
	{
		if (!HasChild())
			return GetRelativePosition();
		
		for (const auto& child : _childs) {
			return GetRelativePosition() + child->GetRelativePosition();
		}
	}

	const glm::quat& TActor::GetWorldRotation() const
	{
		if (!HasChild())
			return GetRelativeRotation();

		for (const auto& child : _childs) {
			return GetRelativeRotation() + child->GetRelativeRotation();
		}
	}

	const glm::vec3& TActor::GetWorldScale() const
	{
		if (!HasChild())
			return GetRelativeScale();

		for (const auto& child : _childs) {
			return GetRelativeScale() + child->GetRelativeScale();
		}
	}

	const Transform& TActor::GetWorldTransform() const
	{
		if (!HasChild())
			return GetRelativeTransform();
		
		for (const auto& child : _childs) {
			return GetRelativeTransform() + child->GetRelativeTransform();
		}
	}

	void TActor::SetRelativeTransform(const Transform& transform) {
		_transform = transform;
	}
}