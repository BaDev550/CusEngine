#pragma once

#include <Engine/Core/Core.h>
#include <Engine/Subsystem/Subsystem.h>
#include <Engine/World/Scene/Scene.h>
#include <Engine/World/System.h>
#include <Engine/World/Systems/GarbageCollector.h>

namespace Tourqe::Engine { // All temp classes
	class ENGINE_API WorldSubsystem : public Subsystem {
	public:
		virtual Runtime::Result OnCreate(Engine* engine) override;
		virtual void OnUpdate() override;
		virtual void OnDestroy() override;
		
		virtual void GetDependencyGraph(DependencyGraph& graph) override;

		[[nodiscard]] GarbageCollector& GetGC() { return _gc; }
		[[nodiscard]] Scene* GetActiveScene() const { return _activeScene; }
	private:
		std::vector<System*> _systems;
		Scene* _activeScene;

		GarbageCollector _gc;
		std::chrono::steady_clock::time_point _lastGcTime; // CHANGE!
		const std::chrono::seconds _gcCooldown{ 20 };
	}; 
}