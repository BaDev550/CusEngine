#pragma once
#include <Runtime/Definitions/Types.h>
#include <string>
#include <unordered_map>
#include <functional>
#include <any>
#include <vector>
#include <typeinfo>

namespace CusEngine::Reflect {
	class Property {
	public:
		std::string Name;
		const std::type_info* TypeID = nullptr;

		using GetPtrFn = const void* (*)(const void*);
		using SetPtrFn = void (*)(void*, const void*);

		GetPtrFn GetPtr = nullptr;
		SetPtrFn SetPtr = nullptr;

		template<typename ClassType, typename VarType>
		static Property Bind(const std::string& name, VarType ClassType::* member) {
			Property prop;
			prop.Name = name;
			prop.TypeID = &typeid(VarType);

			prop.GetPtr = [](const void* instance) -> const void* { return &(static_cast<ClassType*>(instance)->*member); };
			prop.SetPtr = [](void* instance, const void* value) { static_cast<ClassType*>(instance)->*member = *static_cast<VarType>(value); };
			return prop;
		}
	};

	class ClassType {
	public:
		std::string Name;
		usize Size;
		std::string BaseClassName;
		const ClassType* BaseClass = nullptr;

		std::function<void* ()> Instantiate = nullptr;
		std::vector<Property> Properties;

		const Property* GetProperty(const std::string& name) const {
			for (const auto& prop : Properties) {
				if (prop.Name == name) return &prop;
			}
			return nullptr;
		}

		bool IsA(const ClassType* other) const {
			const ClassType* current = this;
			while (current) {
				if (current == other) return true;
				current = current->BaseClass;
			}
			return false;
		}
	};
}