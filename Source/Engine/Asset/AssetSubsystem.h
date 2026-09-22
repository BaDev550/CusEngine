#pragma once
#include <Engine/Core/Core.h>
#include <Engine/Subsystem/Subsystem.h>

namespace CusEngine {
	class ENGINE_API AssetSubsystem final : public Subsystem {
	public:
		virtual Result OnCreate(Engine* engine) override;
		virtual void OnDestroy() override;
		virtual void GetDependencyGraph(DependencyGraph& graph) override;
	};
}