#include "WorldSubsystem.h"

#include <Engine/Scene/Systems/SceneRenderer2D.h>
#include <Engine/Asset/AssetSubsystem.h>
#include <Engine/Renderer/RenderSubsystem.h>

namespace Tourqe::Engine {
	Runtime::Result WorldSubsystem::OnCreate(Engine* engine) {
		Subsystem::OnCreate(engine);

		SceneRenderer2DSystem* renderSystem = Runtime::Mem::Allocator::Construct<SceneRenderer2DSystem>();
		renderSystem->SetEngine(engine);
		renderSystem->OnCreate();

		_systems.push_back(std::move(renderSystem));

		_activeScene = Runtime::Mem::Allocator::Construct<Scene>();

		return Runtime::Result();
	}

	void WorldSubsystem::OnUpdate() {
		for (auto& system : _systems) {
			system->OnUpdate(*_activeScene);
		}
	}

	void WorldSubsystem::OnDestroy() {
		Subsystem::OnDestroy();

		for (auto& system : _systems) {
			system->OnDestroy();
			Runtime::Mem::Allocator::Destroy(system);
		}
		Runtime::Mem::Allocator::Destroy(_activeScene);
	}

	void WorldSubsystem::GetDependencyGraph(DependencyGraph& graph) {
		graph.Require<AssetSubsystem>(DependencyOrder::After);
		graph.Require<RenderSubsystem>(DependencyOrder::After);
	}
}