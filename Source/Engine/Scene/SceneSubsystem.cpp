#include "SceneSubsystem.h"

#include <Engine/Scene/Systems/SceneRenderer2D.h>
#include <Engine/Asset/AssetSubsystem.h>
#include <Engine/Renderer/RenderSubsystem.h>

namespace Tourqe::Engine {
	Runtime::Result SceneSubsystem::OnCreate(Engine* engine) {
		Subsystem::OnCreate(engine);

		SceneRenderer2DSystem* renderSystem = Runtime::Mem::Allocator::Construct<SceneRenderer2DSystem>();
		renderSystem->SetEngine(engine);
		renderSystem->OnCreate();

		_systems.push_back(std::move(renderSystem));

		_activeScene = Runtime::Mem::Allocator::Construct<Scene>();

		return Runtime::Result();
	}

	void SceneSubsystem::OnUpdate() {
		for (auto& system : _systems) {
			system->OnUpdate(*_activeScene);
		}
	}

	void SceneSubsystem::OnDestroy() {
		Subsystem::OnDestroy();

		for (auto& system : _systems) {
			system->OnDestroy();
			Runtime::Mem::Allocator::Destroy(system);
		}
		Runtime::Mem::Allocator::Destroy(_activeScene);
	}

	void SceneSubsystem::GetDependencyGraph(DependencyGraph& graph) {
		graph.Require<AssetSubsystem>(DependencyOrder::After);
		graph.Require<RenderSubsystem>(DependencyOrder::After);
	}
}