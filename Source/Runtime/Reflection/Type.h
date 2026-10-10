#pragma once

#include <Runtime/Definitions/Types.h>
#include <Runtime/Definitions/Logger.h>
#include <Runtime/Memory/Memory.h>
#include <functional>
#include <typeindex>
#include <string>
#include <string_view>

namespace Runtime::Reflection {
	class Type final {
	public:
		Type() = default;
		virtual ~Type() = default;
		Type(const Type&) = delete;
		Type& operator=(const Type&) = delete;
		Type(Type&&) noexcept = default;
		Type& operator=(Type&&) noexcept = default;

		[[nodiscard]] std::type_index GetTypeIndex() const { return _type; }
		[[nodiscard]] std::type_index GetBaseTypeIndex() const { return _baseType; }

		const std::string& GetName() const { return _name; }
		usize GetSize() const { return _size; }
		usize GetAlignment() const { return _alignment; }

		void Construct(void* mem) {
			if (!_constructFunc) {
				Logger::Error("Type", "{} has No default constructor", _name);
				return;
			}
			_constructFunc(mem);
		}
		
		void DestructAt(void* mem) {
			if (!_destructFunc) {
				Logger::Error("Type", "{} has No default deconstructor", _name);
				return;
			}
			_destructFunc(mem);
		}

		void* Create() const {
			if (!_constructFunc) {
				Logger::Error("Type", "{} has no default constructor, cannot be created", _name);
				return nullptr;
			}
			void* mem = Mem::Allocator::Allocate(_size, _alignment);
			_constructFunc(mem);
			return mem;
		}
	private:
		using ConstructFunc = std::function<void(void*)>;
		using DestructFunc = std::function<void(void*)>;

		std::string _name;
		usize _size;
		usize _alignment;
		bool _abstractClass;

		std::type_index _type = typeid(void);
		std::type_index _baseType = typeid(void);

		ConstructFunc _constructFunc;
		DestructFunc _destructFunc;

		template<typename>
		friend class TypeBuilder;
	};
}