#include "SceneRenderer2D.h"

#include <Engine/Asset/AssetSubsystem.h>
#include <Engine/Window/WindowSubsystem.h>
#include <Engine/Renderer/RenderSubsystem.h>

#include <imgui.h>

namespace Tourqe::Engine {
	Runtime::Result SceneRenderer2DSystem::OnCreate() {
		auto* renderSystem = _engine->GetSubsystem<RenderSubsystem>();

		return Runtime::Result();
	}

	void SceneRenderer2DSystem::OnUpdate(Scene& scene) {
		auto renderSystem = _engine->GetSubsystem<RenderSubsystem>();

	}

	void SceneRenderer2DSystem::OnDestroy() { }
}