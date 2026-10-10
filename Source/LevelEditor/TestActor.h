#pragma once

#include <Engine/World/Objects/Actor.h>
#include <Runtime/Definitions/Logger.h>
#include <string>

namespace Tourqe {
	TCLASS()
	class ENGINE_API TTestActor final : public Engine::TActor {
		GENERATE_CLASS(TTestActor)
	public:
		virtual void OnCreated() override {
			Logger::Info("TTestActor", "Created");
		}

		void PrintSomething() {
			Logger::Warn("TTestActor", "Yea, {}", GetObjectID().Str());
		}
	};
}