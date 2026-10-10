#pragma once

#include <Runtime/Reflection/Type.h>
#include <vector>
#include <functional>

namespace Tourqe::Engine {
	class ReflectionSystem final {
	public:
		ReflectionSystem();
		~ReflectionSystem();

		template<typename TBase>
		void ForEachWithBase(std::function<void(const Runtime::Reflection::Type& type)> func) {
			for (auto& type : _types) {
				if (type.GetBaseTypeIndex() == typeid(TBase))
					func(type);
			}
		}
	private:
		std::vector<Runtime::Reflection::Type> _types;
		std::unordered_map<std::type_index, Runtime::Reflection::Type*> _lookupTable;
	};
}