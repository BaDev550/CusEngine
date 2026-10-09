#pragma once

#include <Runtime/Reflection/Type.h>
#include <Runtime/Definitions/Types.h>
#include <Runtime/Definitions/Logger.h>

namespace Runtime::Reflection {
	class TypeRegistry final {
	public:
		TypeRegistry() = default;
		~TypeRegistry() {
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

		template<typename T, typename Base>
		void Register() {
			Type* type = Mem::Allocator::Construct<Type>();
			type->size = sizeof(T);
			type->alignment = alignof(T);
			type->type = typeid(T);
			type->baseType = typeid(Base);
			
			if constexpr (std::is_default_constructible_v<T> && !std::is_abstract_v<T>) {
				type->constructFunc = []() -> void* { return Mem::Allocator::Construct<T>(); };
				type->deconstructFunc = [](void* p) { Mem::Allocator::Destroy(static_cast<T*>(p)); };
			}

			_entiries[type->type] = type;
			Logger::Info("TypeRegistry", "TypeRegistry {} registered", type->GetClassName().data());
		}

		template<class ClassT = Type> requires std::is_base_of_v<Type, ClassT>
		ClassT* GetClass(std::type_index type) {
			if (_entiries.contains(type))
				return static_cast<ClassT*>(_entiries[type]);
			return nullptr;
		}

		static TypeRegistry& Get() {
			static TypeRegistry factory;
			return factory;
		}
	private:
		std::unordered_map<std::type_index, Type*> _entiries;
	};

	template<typename... Rs>
	struct TypeList {
		static void RegisterAll(TypeRegistry& r) {
			(r.template Register<typename Rs::Type, typename Rs::Base>(), ...);
		}
	};
}