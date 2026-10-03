#pragma once

#include <Engine/Core/Core.h>
#include <Engine/Subsystem/Subsystem.h>
#include <Engine/Scene/System.h>

namespace CusEngine { // All temp classes
	class Scene {
	public:
		bool _initialized = false;
	};

	class ENGINE_API SceneSubsystem : public Subsystem {
	public:
		virtual Runtime::Result OnCreate(Engine* engine) override;
		virtual void OnUpdate() override;
		virtual void OnDestroy() override;

		virtual void GetDependencyGraph(DependencyGraph& graph) override;
	private:
		std::vector<System*> _systems;
		Scene* _activeScene;
	};
}