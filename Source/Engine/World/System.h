#pragma once

#include <Engine/Core/Core.h>
#include <Engine/Core/Engine.h>
#include <Runtime/Definitions/Result.h>

namespace Tourqe::Engine {
	class Scene;

	class ENGINE_API System {
	public:
		virtual Runtime::Result OnCreate() = 0;
		virtual void OnUpdate(Scene& scene) = 0;
		virtual void OnDestroy() = 0;

		void SetEngine(Engine* engine) { _engine = engine; }
	protected:
		Engine* _engine;
	};
}