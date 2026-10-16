#pragma once
#include <Engine/World/ECS/EntityFactory.h>

#include <Runtime/Definitions/UUID.h>
#include <unordered_map>

namespace Tourqe::Engine {
	class WorldSubsystem;

	class ENGINE_API Scene final {
	public:
		Scene() = default;

		EntityHandle CreateEntity();
		void DestroyEntity(EntityHandle handle);
		void DestroyEntity(usize index);
		b8 IsValid(EntityHandle handle);
	private:
		EntityFactory _entityFactory;
	};
}