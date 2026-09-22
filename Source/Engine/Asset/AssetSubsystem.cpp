#include "AssetSubsystem.h"
#include <Engine/Renderer/RenderSubsystem.h>
#include <Engine/Subsystem/PluginLoaderSubsystem.h>

namespace CusEngine {
	Result AssetSubsystem::OnCreate(Engine* engine)
	{
		Subsystem::OnCreate(engine);

		return Result();
	}

	void AssetSubsystem::OnDestroy() {
	
	}

	void AssetSubsystem::GetDependencyGraph(DependencyGraph& graph) {
		graph.Require<RenderSubsystem>(DependencyOrder::Before);
		graph.Require<PluginSubsystem>(DependencyOrder::After);
	}
}