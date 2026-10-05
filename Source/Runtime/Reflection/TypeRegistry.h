#pragma once

#include <Runtime/Reflection/Type.h>
#include <Runtime/Definitions/Types.h>
#include <Runtime/Definitions/Logger.h>

namespace Runtime::Reflection {
	class TypeRegistry final {
	public:
		void Register(Type* type) {
			_entiries[type->type] = type;

			Logger::Info("TypeRegistry", "TypeRegistry {} registered", type->GetClassName().data());
		}

		void Shutdown() {
			for (auto& [index, type] : _entiries) {
				Mem::Allocator::Destroy(type);
			}
			_entiries.clear();
		}

		void ForEach(std::function<void(Type* type)> func) const {
			for (const auto& [index, info] : _entiries)
				func(info);
		}

		template<typename T>
		void ForEachForBase(std::function<void(Type* type)> func) const {
			for (const auto& [index, type] : _entiries) {
				if (type->baseType == typeid(T))
					func(type);
			}
		}

		Type* GetClass(std::type_index type) {
			if (_entiries.contains(type))
				return _entiries[type];
			return nullptr;
		}

		static TypeRegistry& Get() {
			static TypeRegistry factory;
			return factory;
		}
	private:
		std::unordered_map<std::type_index, Type*> _entiries;
	};

#define REGISTER_CLASS(ClassType, BaseClassType) \
	namespace { \
		CusEngine::ClassType* ClassType##_create() { \
			auto* type = Runtime::Mem::Allocator::Construct<CusEngine::ClassType>(); \
			return type; \
		} \
		struct ClassType##_register { \
			ClassType##_register() { \
				CusEngine::ClassType* entry = Runtime::Mem::Allocator::Construct<CusEngine::ClassType>(); \
				entry->size = sizeof(CusEngine::ClassType); \
				entry->alignment = alignof(CusEngine::ClassType); \
				entry->type = typeid(CusEngine::ClassType); \
				entry->baseType = typeid(CusEngine::BaseClassType); \
				entry->constructFunc = &ClassType##_create; \
				Runtime::Reflection::TypeRegistry::Get().Register(std::move(entry)); \
			} \
		}; \
		static ClassType##_register s_##Type##_register; \
	}
}