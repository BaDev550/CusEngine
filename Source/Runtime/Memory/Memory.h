#pragma once
#include <Engine/Core/Types.h>
#include <Engine/Core/Logger.h>

#include <memory>
#include <unordered_map>

namespace Runtime::Mem {
	struct MemBlock {
		usize size;
		usize alignment;
	};

	class MemoryTracker final {
	public:
		void Record(void* ptr, MemBlock block) {
			usize actualSize = sizeof(ptr);
			usize newSize = actualSize + sizeof(MemBlock);

			std::memcpy(ptr, &block, sizeof(MemBlock));
		}

		[[nodiscard]] MemBlock Release(void* ptr) {
			MemBlock block;
		}
	};

	template<class T>
	using Unique = std::unique_ptr<T>;

	class Allocator final {
	public:
		static void* Allocate(usize size, usize align = 16) {
			MemBlock block;
			block.alignment = std::align_val_t(align);
			block.size = size;
			
			_tracker.Record(mem, { size, align });
			return mem;
		}

		static void Free(void* ptr) {
			if (!ptr) return;

			MemBlock block = _tracker.Release(ptr);
			::operator delete(ptr, std::align_val_t(block.alignment));
		}

		template<typename T, typename... Args>
		static T* Construct(Args&&... args) {
			void* mem = Allocate(sizeof(T), alignof(T));
			return new (mem) T(std::forward<Args>(args)...);
		}

		template<typename T>
		static void Destroy(T* ptr) {
			if (!ptr) return;
			void* base = ptr;

			if constexpr (std::is_polymorphic_v<T>)
				base = dynamic_cast<void*>(ptr);

			ptr->~T();
			Free(base);
			ptr = nullptr;
		}

		template<typename T, typename... Args>
		static Unique<T> ConstructUnique(Args&&... args) {
			return std::make_unique<T>(std::forward<Args>(args)...);
		}

		static MemoryTracker GetTracker() { return _tracker; }
	private:
		static inline MemoryTracker _tracker;
	};
}