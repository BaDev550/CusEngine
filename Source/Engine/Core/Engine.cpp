#include "Engine.h"
#include "Core/Logger.h"

namespace CusEngine {
	Engine::Engine() {}
	Engine::~Engine() { Logger::Info("Engine", "Shuting down..."); }

	void Engine::Run() {
		Logger::Info("Engine", "Engine running...");

		SortAndInitializeSystems();

		// TODO(0x): add bubble sort to handle pending init list
		for (const auto& subsystem : _activeSubsystemList) {
			subsystem->OnCreate(this);
		}

		while (_running) {

			for (const auto& subsystem : _activeSubsystemList) {
				subsystem->OnUpdate();
			}
		}

		for (const auto& subsystem : _activeSubsystemList) {
			subsystem->OnDestroy();
		}
	}

	void Engine::Shutdown(const std::string_view reson) {
		Logger::Info("Engine", "Closing reson: {}", reson.data());
		_running = false;
	}

	void Engine::SortAndInitializeSystems() {
		for (int i = 0; i < _pendingInitList.size(); i++) { // TODO(0x): fix this bitch ass O(n2)
			Subsystem* currSystem = _pendingInitList.at(i);

			DependencyGraph currentGraph;
			currSystem->GetDependencyGraph(currentGraph);

			DependencyGraph::DependencyList currentList = currentGraph.GetList();

			for (auto& [index, order] : currentList) {
				Logger::Info(
					currSystem->GetTypeID().name(), 
					"Dependent on: {}", _pendingInitList[_systemInitLookupTable[index]]->GetTypeID().name());
			}
		}
	}
}