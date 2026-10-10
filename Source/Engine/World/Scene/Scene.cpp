#include "Scene.h"

#include <Engine/Core/Engine.h>
#include <Engine/Reflection/ReflectionSystem.h>
#include <Engine/World/WorldSubsystem.h>
#include <Runtime/Reflection/Type.h>

namespace Tourqe::Engine {
	TActor* Scene::SpawnActorFromClass(const std::string& className, const Transform& spawnTransform) {

		if (auto* engine = Engine::Get()) {
			Runtime::Reflection::Type* type = Engine::Get()->GetReflectionSystem()->GetType(className);
			Runtime::UUID actorID = Runtime::UUID();
			TActor* actor = static_cast<TActor*>(type->Create());
			if (actor) {
				_actors[actorID] = std::move(actor);
				_actors[actorID]->_objectID = actorID;
				_actors[actorID]->_worldContext = _ownerWorld;
				_actors[actorID]->SetRelativeTransform(spawnTransform);
				_actors[actorID]->OnCreated();
				return actor;
			}
			return nullptr;
		}
		return nullptr;
	}

	void Scene::DestroyActor(TActor* actor) {
		if (_actors.contains(actor->GetObjectID())) {
			_ownerWorld->GetGC().DestroyObject(actor);

			actor->OnDestroy();
			_actors.erase(actor->GetObjectID());
		}
	}
}