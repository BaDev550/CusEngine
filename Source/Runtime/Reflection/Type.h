#pragma once

#include <Runtime/Definitions/Types.h>
#include <functional>
#include <typeindex>
#include <string_view>

namespace Runtime::Reflection {

#define CCLASS(...)
#define CPROP(...)

#define GENERATE_CLASS(ClassType) \
public: \ 
	using Self = ClassType; \
	static constexpr std::string_view StaticClassName() { return #ClassType; }

	template<typename T, typename B = void>
	struct Reflected {
		static_assert(std::is_void_v<B> || std::is_base_of_v<B, T>, "CCLASS base mismatch");
		using Type = T;
		using Base = B;
	};

	class Type {
	public:
		virtual ~Type() = default;
		virtual std::string_view GetClassName() { return type.name(); }

		usize size;
		usize alignment;

		std::type_index type = typeid(void);
		std::type_index baseType = typeid(void);

		std::function<void*()> constructFunc;
		std::function<void(void* p)> deconstructFunc;
	};
}