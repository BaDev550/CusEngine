#include "RenderSubsystem.h"
#include "Core/Engine.h"

#include "Window/WindowSubsystem.h"
#include "Graphics/RHI/RHISubsystem.h"
#include "MT/JobSubsystem.h"

namespace CusEngine {
	bool RenderSubsystem::OnCreate(Engine* engine) {
		Subsystem::OnCreate(engine);

		auto window = _engine->GetSubsystem<WindowSubsystem>()->GetWindow();
		auto rhi = _engine->GetSubsystem<RHI::RHISubsystem>();
		
		_commands = rhi->CreateRenderCommands(window->GetRenderContext(), window->GetSwapchain());
		return true;
	}

	void RenderSubsystem::OnUpdate() {
		
	}

	void RenderSubsystem::OnDestroy() {
		auto rhi = _engine->GetSubsystem<RHI::RHISubsystem>();

		rhi->DestroyRenderCommands(_commands);
	}

	void RenderSubsystem::GetDependencyGraph(DependencyGraph & graph) {
		graph.Require<WindowSubsystem>(DependencyOrder::After);
		graph.Require<MT::JobSubsystem>(DependencyOrder::After);
	}
}