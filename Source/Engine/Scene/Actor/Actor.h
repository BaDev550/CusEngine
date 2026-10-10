#pragma once

#include <Engine/Core/Core.h>
#include <Engine/Core/Object.h>

#include <glm/gtc/quaternion.hpp>
#include <glm/glm.hpp>

namespace Tourqe::Engine {
	class WorldSubsystem;

	TCLASS()
	class ENGINE_API TActor : public TObject {
		GENERATE_CLASS(TActor)
	public:
		TActor() = default;
		virtual ~TActor() = default;

		virtual void OnCreated() {};
		virtual void OnUpdate() {};
		virtual void OnFixedUpdate() {};
		virtual void OnDestroy() {};

		void SetRotation(const glm::vec3& rot);

		__forceinline const glm::vec3& GetPosition() const { return _position; }
		__forceinline const glm::quat& GetRotation() const { return _rotation; }
		__forceinline const glm::vec3& GetScale() const { return _scale; }
	protected:
		virtual WorldSubsystem* GetWorld() const { return _worldContext; }
	private:
		WorldSubsystem* _worldContext;

		glm::vec3 _position;
		glm::quat _rotation;
		glm::vec3 _rotationEuler;
		glm::vec3 _scale;
	};
}