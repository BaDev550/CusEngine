#pragma once

#include <Runtime/Definitions/Types.h>
#include <Runtime/Reflection/Macros.h>
#include <string_view>

namespace Runtime::Reflection {
	class ENGINE_API ReflectObject {
	public:
		virtual std::string_view GetClassName() = 0;
	};
}