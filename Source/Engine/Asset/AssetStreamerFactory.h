#pragma once

#include <Engine/Core/Core.h>
#include <Runtime/Definitions/Logger.h>
#include <functional>
#include <typeindex>

namespace Tourqe::Engine {
	class AssetStreamer;

	struct AssetStreamerEntry {
		usize size;
		usize alignment;
		std::type_index type = typeid(void);
		std::function<AssetStreamer*()> constructFunc;
	};

	class AssetStreamerFactory final {
	public:
		void Register(const AssetStreamerEntry& entry) {
			_entiries[entry.type] = entry;
			Logger::Info("AssetStreamerFactory", "AssetStreamer {} registered", entry.type.name());
		}

		void ForEach(std::function<void(const AssetStreamerEntry& info)> func) const {
			for (const auto& [index, info] : _entiries)
				func(info);
		}

		static AssetStreamerFactory& Factory() {
			static AssetStreamerFactory factory;
			return factory;
		}
	private:
		std::unordered_map<std::type_index, AssetStreamerEntry> _entiries;
	};

#define REGISTER_ASSETSTREAMER(Type) \
	namespace { \
		Tourqe::Engine::AssetStreamer* Type##_create() { \
			auto* streamer = Runtime::Mem::Allocator::Construct<Tourqe::Engine::Type>(); \
			return streamer; \
		} \
		struct Type##_register { \
			Type##_register() { \
				Tourqe::Engine::AssetStreamerEntry entry{}; \
				entry.size = sizeof(Tourqe::Engine::Type); \
				entry.alignment = alignof(Tourqe::Engine::Type); \
				entry.type = typeid(Tourqe::Engine::Type); \
				entry.constructFunc = &Type##_create; \
				Tourqe::Engine::AssetStreamerFactory::Factory().Register(entry); \
			} \
		}; \
		static Type##_register s_##Type##_register; \
	}
}