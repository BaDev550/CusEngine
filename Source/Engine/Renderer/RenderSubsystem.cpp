#include "RenderSubsystem.h"
#include "Core/Engine.h"

#include "Window/WindowSubsystem.h"
#include "MT/JobSubsystem.h"

namespace CusEngine {
	bool RenderSubsystem::OnCreate(Engine* engine) {
		Subsystem::OnCreate(engine);

		auto window = engine->GetSubsystem<WindowSubsystem>()->GetWindow();

		_commands = RHI::CreateRenderCommands(window->GetRenderContext(), window->GetSwapchain());

		Logger::Info("RenderSubsystem", "Created!");
		return true;
	}

	void RenderSubsystem::OnUpdate() {
	
	}

	void RenderSubsystem::OnDestroy() {
	
	}

	void RenderSubsystem::GetDependencyGraph(DependencyGraph & graph) {
		graph.Require<WindowSubsystem>(DependencyOrder::After);
		graph.Require<MT::JobSubsystem>(DependencyOrder::After);
	}
}