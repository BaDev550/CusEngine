#include "Engine.h"
#include "Core/Logger.h"
#include <queue>

#include "Subsystem/PluginLoaderSubsystem.h"
#include "Object.h"
#include "Reflection/TypeRegistry.h"
#include "Reflection/TypeDescriptor.h"
#include "Graphics/RHI/RHISubsystem.h"

namespace CusEngine {
	Engine::Engine() {}
	Engine::~Engine() { Logger::Info("Engine", "Shuting down..."); }

	void Engine::Run() {
		Logger::Info("Engine", "Engine running...");

		PreInitializePlugins();
		SortAndInitializeSystems();

		while (_running) {

			for (const auto& subsystem : _activeSubsystemList) {
				subsystem->OnUpdate();
			}
		}

		for (int i = _activeSubsystemList.size(); i == 0; i++) {
			Subsystem* system = _activeSubsystemList.at(i);
			if (system) {
				system->OnDestroy();
				delete system;
			}
		}
		_activeSubsystemList.clear();
	}

	void Engine::Shutdown(const std::string_view reson) {
		Logger::Info("Engine", "Closing reson: {}", reson.data());
		_running = false;
	}

	void Engine::PreInitializePlugins() {
		PluginSubsystem pluginLoader;
		pluginLoader.LoadPlugin("RHI_Vulkan.dll");

		auto* vulkanInstance = Reflect::TypeRegistry::Get().Create<Subsystem>("Vulkan_RHISubsystem");
		if (vulkanInstance) {
			AddSubsystem<RHI::RHISubsystem>(vulkanInstance);
		}
		else {
			Logger::Error("Engine", "Failed to reflect Vulkan_RHISubsystem from RHI_Vulkan.dll");
		}
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
		std::vector<usize> oldToNewIndex(numSystems);
		sortedList.reserve(numSystems);

		while (!readyQueue.empty()) {
			usize u = readyQueue.front();
			readyQueue.pop();

			oldToNewIndex[u] = sortedList.size();
			sortedList.push_back(_pendingInitList[u]);

			for (usize v : adjList[u]) {
				inDegree[v]--;
				if (inDegree[v] == 0)
					readyQueue.push(v);
			}
		}

		Logger::Assert((sortedList.size() == numSystems), "Engine", "Circular dependency detected in Subsystem initialization!");
		for (const auto& [typeIdx, oldPendingIdx] : _systemInitLookupTable) {
			_systemLookupTable[typeIdx] = oldToNewIndex[oldPendingIdx];
		}

		_activeSubsystemList = std::move(sortedList);
		_pendingInitList.clear();
		_systemInitLookupTable.clear();

		for (auto system : _activeSubsystemList) {
			system->OnCreate(this);
		}

		_initialized = true;
	}

	void Engine::RegisterSubsystem(std::type_index type, Subsystem* instance) {
		if (!instance) {
			Logger::Error("EngineSubsystem", "Attemped to register null subsystem");
			return;
		}

		if (_systemLookupTable.find(type) != _systemLookupTable.end() || _systemInitLookupTable.find(type) != _systemInitLookupTable.end()) {
			Logger::Warn("EngineSubsystem", "Subsystem {} is already registered.", type.name());
			return;
		}

		if (_initialized) {
			InitializeSingleSubsystem(type, instance);
		}
		else {
			usize pendingIdx = _pendingInitList.size();
			_systemInitLookupTable[type] = pendingIdx;
			_systemInitLookupTable[instance->GetTypeID()] = pendingIdx;

			_pendingInitList.push_back(instance);
			Logger::Info("EngineSubsystem", "Subsystem {} queued for startup initialization.", type.name());
		}
	}
	
	void Engine::InitializeSingleSubsystem(std::type_index type, Subsystem* instance) {
		DependencyGraph graph;
		instance->GetDependencyGraph(graph);

		for (const auto& [depType, order] : graph.GetList()) {
			if (_systemLookupTable.find(depType) == _systemLookupTable.end()) {
				Logger::Warn("EngineSubsystem", "Runtime Subsystem {} is missing dependency {}!", type.name(), depType.name());
			}
		}

		usize newActiveIndex = _activeSubsystemList.size();
		_systemLookupTable[type] = newActiveIndex;
		_systemLookupTable[instance->GetTypeID()] = newActiveIndex;

		_activeSubsystemList.push_back(instance);
		instance->OnCreate(this);

		Logger::Info("EngineSubsystem", "Subsystem {} initialized dynamically at runtime.", type.name());
	}
}