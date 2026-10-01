#include <Engine/Core/Engine.h>
#include <Runtime/Definitions/Logger.h>

#include <queue>

#include <Runtime/IO/FileBuffer.h>

namespace CusEngine {
	Engine* Engine::_instance = nullptr;

	Engine::Engine() {
		Logger::Assert(!_instance, "Engine", "NO 2nd INSTANCE OF ENGINE!!");

		_instance = this;
		Logger::Info("Engine", "Created");
	}
	Engine::~Engine() { Logger::Info("Engine", "Shuting down..."); }

	void Engine::Run() {
		Logger::Info("Engine", "Engine running...");

		SortAndInitializeSystems();

		while (_running) {

			for (const auto& subsystem : _activeSubsystemList) {
				subsystem->OnUpdate();
			}
		}

		for (auto it = _activeSubsystemList.rbegin(); it != _activeSubsystemList.rend(); ++it) {
			Subsystem* system = *it;
			if (system) {
				system->OnDestroy();
				Runtime::Mem::Allocator::Destroy(system);
			}
		}
	}

	void Engine::Shutdown(const std::string_view reson) {
		Logger::Info("Engine", "Closing reson: {}", reson.data());
		_running = false;
	}

	void Engine::SortAndInitializeSystems() {
		const usize numSystems = _pendingInitList.size();

		std::vector<std::vector<usize>> adjList(numSystems);
		std::vector<int> inDegree(numSystems, 0);

		for (usize i = 0; i < numSystems; i++) {
			Subsystem* currSystem = _pendingInitList[i];
			std::string currSystemName = currSystem->GetTypeID().name();

			DependencyGraph graph;
			currSystem->GetDependencyGraph(graph);

			for (const auto& [depType, order] : graph.GetList()) {
				auto it = _systemInitLookupTable.find(depType);
				if (it == _systemInitLookupTable.end()) {
					Logger::Warn(currSystemName, "Dependency {} is not in the lookup table!", depType.name());
					continue;
				}
				usize depIndex = it->second;

				if (depIndex >= numSystems || i >= numSystems) { continue; }

				if (order == DependencyOrder::Before) {
					adjList[i].push_back(depIndex);
					inDegree[depIndex]++;
				}
				else if (order == DependencyOrder::After) {
					adjList[depIndex].push_back(i);
					inDegree[i]++;
				}
			}
		}

		std::queue<usize> readyQueue;
		for (usize i = 0; i < numSystems; i++) {
			if (inDegree[i] == 0)
				readyQueue.push(i);
		}

		std::vector<Subsystem*> sortedList;
		sortedList.reserve(numSystems);

		while (!readyQueue.empty()) {
			usize u = readyQueue.front();
			readyQueue.pop();

			sortedList.push_back(_pendingInitList[u]);
			for (usize v : adjList[u]) {
				inDegree[v]--;
				if (inDegree[v] == 0)
					readyQueue.push(v);
			}
		}

		Logger::Assert((sortedList.size() == numSystems), "Engine", "Circular dependency detected in Subsystem initialization!");
		_pendingInitList = std::move(sortedList);

		for (Subsystem* system : _pendingInitList) {
			if (Runtime::Result result = system->OnCreate(this); !result) {
				Logger::Error(system->GetTypeID().name(), "Failed to Create reson: {}", result.GetMessage());
			}
			else {
				_systemLookupTable[system->GetTypeID()] = _activeSubsystemList.size();
				_activeSubsystemList.push_back(system);
				//Runtime::Mem::Allocator::GetTracker().Record(_activeSubsystemList[_activeSubsystemList.size() - 1], { sizeof(*system), alignof(Subsystem)} );
			}
		}
		_pendingInitList.clear();
	}

	Engine* Engine::Get() { return _instance; }
}