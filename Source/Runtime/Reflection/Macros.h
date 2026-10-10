#pragma once

#include <string_view>

namespace Runtime::Reflection {
	template<typename>
	struct TypeAccessor;
}

#define TCLASS(...)
#define TPROP(...)

#define GENERATE_CLASS(ClassType) \
public: \
	template<typename> \
	friend struct Runtime::Reflection::TypeAccessor; \
	virtual std::string_view GetClassName() override { return #ClassType; } \
	static constexpr std::string_view StaticClassName() { return #ClassType; }
