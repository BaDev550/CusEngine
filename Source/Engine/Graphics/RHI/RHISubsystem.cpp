#include "RHISubsystem.h"
#include "Window/WindowSubsystem.h"
#include "Subsystem/PluginLoaderSubsystem.h"

namespace CusEngine::RHI {
	void RHISubsystem::GetDependencyGraph(DependencyGraph& graph) {
		graph.Require<WindowSubsystem>(DependencyOrder::After);
	}
}