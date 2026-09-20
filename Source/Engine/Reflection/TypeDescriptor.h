#pragma once
#include "Core/Types.h"
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

		std::function<std::any(void*)> Get;
		std::function<void(void*, std::any)> Set;

		template<typename ClassType, typename VarType>
		static Property Bind(const std::string& name, VarType ClassType::* member) {
			Property prop;
			prop.Name = name;
			prop.TypeID = &typeid(VarType);

			prop.Get = [member](void* instance) -> std::any {
				return static_cast<ClassType*>(instance)->*member;
				};
			prop.Set = [member](void* instance, std::any value) {
				static_cast<ClassType*>(instance)->*member = std::any_cast<VarType>(value);
				};
			return prop;
		}
	};

	class ClassType {
	public:
		std::string Name;
		usize Size;

		std::function<void* ()> Instantiate = nullptr;
		std::vector<Property> Properties;

		const Property* GetProperty(const std::string& name) const {
			for (const auto& prop : Properties) {
				if (prop.Name == name) return &prop;
			}
			return nullptr;
		}
	};
}