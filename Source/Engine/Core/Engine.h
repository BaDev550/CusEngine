#pragma once
#include "Core/Memory.h"
#include "Subsystem/Subsystem.h"

#include <unordered_map>
#include <string_view>

namespace CusEngine {
	class Engine final {
	public:
		Engine();
		~Engine();
		Engine(const Engine&) = delete;
		Engine& operator=(const Engine&) = delete;

		void Run();
		void Shutdown(const std::string_view reson);

		template<class T, typename... Args>
		void AddSubsystem(Args&&... args) {
			std::type_index typeindex = typeid(T);
			if (auto it = _systemLookupTable.find(typeindex); it != _systemLookupTable.end()) {
				Logger::Warn("EngineSubsystem", "Tried to add subsystem witch is already inside of the list.");
				return;
			}
			_pendingInitList.push_back(Mem::Allocator::Construct<T>(std::forward<Args>(args)...));
			_systemInitLookupTable[typeindex] = _pendingInitList.size();
			Logger::Info("EngineSubsystem", "Subsystem {} added to engine.", typeindex.name());
		}

		template<class T>
		T* GetSubsystem() {
			std::type_index typeindex = typeid(T);
			if (auto it = _systemLookupTable.find(typeindex); it != _systemLookupTable.end()) {
				return _activeSubsystemList[it.secound];
			}
			Logger::Error("EngineSubsystem", "Failed to find subsystem {}", typeindex.name());
			return nullptr;
		}
	private:
		void SortAndInitializeSystems();

		bool _running = true;

		std::vector<Subsystem*> _pendingInitList;
		std::unordered_map<std::type_index, usize> _systemInitLookupTable;

		std::vector<Subsystem*> _activeSubsystemList;
		std::unordered_map<std::type_index, usize> _systemLookupTable;

		std::vector<Subsystem*> _pendingDestroyList;
	};
}