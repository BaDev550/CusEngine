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
		void AddSubsystem(Args&&...) {
			type_info info = typeid(T);
			if (auto it = _pendingInitList.find(info); it != _pendingInitList.end()) {
				Logger::Warn("EngineSubsystem", "Tried to add subsystem witch is already inside of the list.");
				return;
			}
			_pendingInitList[info] = Mem::Allocator::Construct<T>(std::forward<Args>(args)...);
			Logger::Info("EngineSubsystem", "Subsystem {} added to engine.", info.name);
		}

		template<class T>
		T* GetSubsystem() {
			type_info info = typeid(T);
			if (auto it = _pendingInitList.find(info); it != _pendingInitList.end()) {
				return it->second;
			}
			Logger::Error("EngineSubsystem", "Failed to find subsystem {}", info.name);
			return nullptr;
		}
	private:
		bool _running = true;

		std::unordered_map<type_info, Subsystem*> _pendingInitList;
		std::unordered_map<type_info, Subsystem*> _activeSubsystemList;
		std::unordered_map<type_info, Subsystem*> _pendingDestroyList;
	};
}