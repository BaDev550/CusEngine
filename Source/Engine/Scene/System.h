#pragma once

#include <Engine/Core/Core.h>
#include <Engine/Core/Engine.h>
#include <Runtime/Definitions/Result.h>

namespace CusEngine {
	class Scene;

	class ENGINE_API System {
	public:
		virtual Runtime::Result OnCreate() = 0;
		virtual void OnUpdate(Scene& scene) = 0;
		virtual void OnDestroy() = 0;
	protected:
		Engine* _engine;
	};
}