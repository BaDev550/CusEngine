#pragma once
#include <Engine/World/Objects/Object.h>
#include <Engine/World/Objects/Actor.h>

#include <Runtime/Definitions/UUID.h>
#include <unordered_map>

namespace Tourqe::Engine {
	class WorldSubsystem;

	class ENGINE_API Scene final {
	public:
		Scene() = default;

		TActor* SpawnActorFromClass(const std::string& className, const Transform& spawnTransform);
		void DestroyActor(TActor* actor);

		void SetWorld(WorldSubsystem* world) { _ownerWorld = world; }
	private:
		WorldSubsystem* _ownerWorld;

		std::unordered_map<Runtime::UUID, TActor*> _actors;
	};
}