#pragma once

#include <Runtime/Reflection/Type.h>

namespace Runtime::Reflection {
	template<typename T>
	class TypeBuilder final {
	public:
		static TypeBuilder ForType(std::string name) {
			TypeBuilder builder = {};
			builder._type._name = std::move(name);
			builder._type._type = typeid(T);
			builder._type._size = sizeof(T);
			builder._type._alignment = alignof(T);
			builder._type._abstractClass = std::is_abstract_v<T>;

			if constexpr (!std::is_abstract_v<T>) {
				builder._type._destructFunc = [](void* mem) {
					static_cast<T*>(mem)->~T();
					};

				if constexpr (std::is_default_constructible_v<T>) {
					builder._type._constructFunc = [](void* mem) {
						::new (mem) T();
						};
				}
			}
			return builder;
		}

		template<typename TBase>
		TypeBuilder& Base() {
			_type._baseType = typeid(TBase);
			return *this;
		}

		Type Build() { return std::move(_type); }
	private:
		Type _type;
	};
}