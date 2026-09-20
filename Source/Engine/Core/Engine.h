#pragma once
#include "Core/Memory.h"
#include "Subsystem/Subsystem.h"

#include <unordered_map>
#include <string_view>

namespace CusEngine {
	class ENGINE_API Engine final {
	public:
		Engine();
		~Engine();
		Engine(const Engine&) = delete;
		Engine& operator=(const Engine&) = delete;

		static Engine& Get() { 
			static Engine instance;
			return instance;
		}

		void Run();
		void Shutdown(const std::string_view reson);

		template<class T, typename... Args> requires (!std::is_pointer_v<T>&& std::is_constructible_v<T, Args...>)
		void AddSubsystem(Args&&... args) {
			T* instance = Mem::Allocator::Construct<T>(std::forward<Args>(args)...);
			RegisterSubsystem(typeid(T), instance);
		}

		template<class BaseType = Subsystem>
		BaseType* AddSubsystem(Subsystem* instance) {
			RegisterSubsystem(typeid(BaseType), instance);
			return static_cast<BaseType*>(instance);
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
		void RegisterSubsystem(std::type_index type, Subsystem* instance);
		void SortAndInitializeSystems();
		void InitializeSingleSubsystem(std::type_index type, Subsystem* instance);
		void PreInitializePlugins();

		bool _running = true;
		bool _initialized = false;

		std::vector<Subsystem*> _pendingInitList;
		std::unordered_map<std::type_index, usize> _systemInitLookupTable;

		std::vector<Subsystem*> _activeSubsystemList;
		std::unordered_map<std::type_index, usize> _systemLookupTable;

		std::vector<Subsystem*> _pendingDestroyList;
	};
}