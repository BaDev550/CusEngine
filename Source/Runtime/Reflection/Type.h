#pragma once

#include <Runtime/Definitions/Types.h>
#include <functional>
#include <typeindex>
#include <string_view>

namespace Runtime::Reflection {
	class Type {
	public:
		virtual ~Type() = default;
		virtual std::string_view GetClassName() { return type.name(); }

		usize size;
		usize alignment;

		std::type_index type = typeid(void);
		std::type_index baseType = typeid(void);

		std::function<void*()> constructFunc;
		std::function<void()> deconstructFunc;
	};
}