#pragma once

#include <Engine/Core/Core.h>

#include <Engine/World/Objects/Object.h>
#include <Runtime/Math/Transform.h>
#include <list>

namespace Tourqe::Engine {
	TCLASS()
	class ENGINE_API TActor : public TObject {
		GENERATE_CLASS(TActor)
	public:
		TActor() = default;
		virtual ~TActor() = default;

		virtual void OnUpdate() {};
		virtual void OnFixedUpdate() {};

		virtual void OnCreated() {
			_objectState = ObjectState::Spawned;
		};

		virtual void OnDestroy() {
			_objectState = ObjectState::PendingDestroy;
		};

		void SetRelativeTransform(const Transform& transform);

		const glm::vec3& GetWorldPosition() const; // Not optimal
		const glm::quat& GetWorldRotation() const;
		const glm::vec3& GetWorldScale() const;
		const Transform& GetWorldTransform() const;

		__forceinline const glm::vec3& GetRelativePosition() const { return _transform.position; }
		__forceinline const glm::quat& GetRelativeRotation() const { return _transform.rotation; }
		__forceinline const glm::vec3& GetRelativeScale() const { return _transform.scale; }
		const Transform& GetRelativeTransform() const { return _transform; }

		[[nodiscard]] virtual TActor* GetOuter() { return _outer; }

		[[nodiscard]] inline b8 HasChild() const { return _childs.empty(); }
	private:
		Transform _transform;

		TActor* _outer;
		std::list<TActor*> _childs;
	};
}