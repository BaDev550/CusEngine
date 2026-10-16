#include "Scene.h"

#include <Engine/Core/Engine.h>
#include <Engine/Reflection/ReflectionSystem.h>
#include <Engine/World/WorldSubsystem.h>
#include <Runtime/Reflection/Type.h>

namespace Tourqe::Engine {
	EntityHandle Scene::CreateEntity() {
		return _entityFactory.Create();
	}

	void Scene::DestroyEntity(EntityHandle handle) {
		if (!_entityFactory.IsValid(handle))
			return;

		//const Signature* signature = _entityFactory.GetSignatureOf(handle);
		_entityFactory.Destroy(handle);
	}

	void Scene::DestroyEntity(usize index) {
		EntityHandle handle = _entityFactory.GetHandleAt(index);
		DestroyEntity(handle);
	}

	b8 Scene::IsValid(EntityHandle handle) { return _entityFactory.IsValid(handle); }
}