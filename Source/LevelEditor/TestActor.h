#pragma once

#include <Engine/Scene/Actor/Actor.h>
#include <Runtime/Definitions/Logger.h>

namespace Tourqe {
	TCLASS()
	class ENGINE_API TTestActor final : public Engine::TActor {
		GENERATE_CLASS(TTestActor)
	public:
		virtual void OnCreated() override {
			Logger::Info("TTestActor", "Created");
		}
	};
}