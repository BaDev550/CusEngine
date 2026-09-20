#include "RenderSubsystem.h"
#include <Engine/Core/Engine.h>
#include <Engine/Window/WindowSubsystem.h>

namespace CusEngine {
	bool RenderSubsystem::OnCreate(Engine* engine) {
		Subsystem::OnCreate(engine);

		auto window = engine->GetSubsystem<WindowSubsystem>()->GetWindow();

		//_commands = RHI::CreateRenderCommands(window->GetRenderContext(), window->GetSwapchain());

		return true;
	}

	void RenderSubsystem::OnUpdate() {
		
	}

	void RenderSubsystem::OnDestroy() {
	
	}

	void RenderSubsystem::GetDependencyGraph(DependencyGraph & graph) {
		graph.Require<WindowSubsystem>(DependencyOrder::After);
	}
}