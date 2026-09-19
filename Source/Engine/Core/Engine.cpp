#include "Engine.h"
#include "Core/Logger.h"

namespace CusEngine {
	Engine::Engine() {}
	Engine::~Engine() { Logger::Info("Engine", "Shuting down..."); }

	void Engine::Run() {
		Logger::Info("Engine", "Engine running...");

		// TODO(0x): add bubble sort to handle pending init list
		for (const auto& [name, subsystem] : _activeSubsystemList) {
			subsystem->OnCreate(this);
		}

		while (_running) {

			for (const auto& [name, subsystem] : _activeSubsystemList) {
				subsystem->OnUpdate();
			}
		}

		for (const auto& [name, subsystem] : _activeSubsystemList) {
			subsystem->OnDestroy();
		}
	}

	void Engine::Shutdown(const std::string_view reson) {
		Logger::Info("Engine", "Closing reson: {}", reson.data());
		_running = false;
	}
}