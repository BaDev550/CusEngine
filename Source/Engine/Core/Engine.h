#pragma once
#include <Runtime/Memory/Memory.h>
#include <Engine/Subsystem/Subsystem.h>

#include <unordered_map>
#include <string_view>

namespace Tourqe::Engine {
	class ReflectionSystem;
	class PluginSystem;
	class JobSystem;

	class ENGINE_API Engine final {
	public:
		Engine();
		~Engine();
		Engine(const Engine&) = delete;
		Engine& operator=(const Engine&) = delete;

		static Engine* Get();

		void Run();
		void Shutdown(const std::string_view reson);

		template<class T, typename... Args>
		void AddSubsystem(Args&&... args) {
			std::type_index typeindex = typeid(T);
			if (auto it = _systemLookupTable.find(typeindex); it != _systemLookupTable.end()) {
				Logger::Warn("EngineSubsystem", "Tried to add subsystem witch is already inside of the list.");
				return;
			}
			_systemInitLookupTable[typeindex] = _pendingInitList.size();
			_pendingInitList.push_back(Runtime::Mem::Allocator::Construct<T>(std::forward<Args>(args)...));
			Logger::Info("EngineSubsystem", "Subsystem {} added to engine.", typeindex.name());
		}

		template<class T>
		T* GetSubsystem() {
			std::type_index typeindex = typeid(T);
			if (auto it = _systemLookupTable.find(typeindex); it != _systemLookupTable.end()) {
				return static_cast<T*>(_activeSubsystemList[it->second]);
			}
			Logger::Error("EngineSubsystem", "Failed to find subsystem {}", typeindex.name());
			return nullptr;
		}
	private:
		void SortAndInitializeSystems();
		static Engine* _instance;

		bool _running = true;

		std::vector<Subsystem*> _pendingInitList;
		std::unordered_map<std::type_index, usize> _systemInitLookupTable;

		std::vector<Subsystem*> _activeSubsystemList;
		std::unordered_map<std::type_index, usize> _systemLookupTable;

		std::vector<Subsystem*> _pendingDestroyList;

		ReflectionSystem* _reflectionSystem = nullptr;
		PluginSystem* _pluginSystem = nullptr;
		JobSystem* _jobSystem = nullptr;
	};
}