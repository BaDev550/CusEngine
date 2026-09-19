#include "Engine.h"
#include "Core/Logger.h"
#include <queue>

namespace CusEngine {
	Engine::Engine() {}
	Engine::~Engine() { Logger::Info("Engine", "Shuting down..."); }

	void Engine::Run() {
		Logger::Info("Engine", "Engine running...");

		SortAndInitializeSystems();

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

		for (auto system : _pendingInitList) {
			system->OnCreate(this);

			_activeSubsystemList.push_back(system);
		}
		_pendingInitList.clear();
	}
}