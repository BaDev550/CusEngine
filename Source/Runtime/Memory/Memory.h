#pragma once
#include <Runtime/Definitions/Types.h>
#include <Runtime/Definitions/Logger.h>

#include <memory>
#include <unordered_map>

namespace Runtime::Mem {
	struct MemBlock {
		usize size;
		usize alignment;
	};
	
	namespace {
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

		usize AlignUp(usize value, usize align) { return (value + (align - 1)) & ~(align - 1); }
	}

	template<class T>
	using Unique = std::unique_ptr<T>;

	class Allocator final {
	public:
		static void* Allocate(usize size, usize align = 16) {
			const usize eff = align < alignof(std::max_align_t) ? alignof(std::max_align_t) : align;
			const usize headerSize = AlignUp(sizeof(MemBlock), eff);

			void* base = ::operator new(headerSize + size, std::align_val_t{ eff }, std::nothrow);
			if (!base) {
				Logger::Fatal("Allocator", "Out of memory requesting {} bytes", size);
				return nullptr;
			}

			void* user = static_cast<c8*>(base) + headerSize;

			MemBlock* pHeader = reinterpret_cast<MemBlock*>(static_cast<c8*>(user) - sizeof(MemBlock));
			pHeader->size = size;
			pHeader->alignment = eff;

			return user;
		}

		static void Free(void* ptr) {
			if (!ptr)
				return;

			MemBlock* pHeader = reinterpret_cast<MemBlock*>(static_cast<c8*>(ptr) - sizeof(MemBlock));
			const usize eff = pHeader->alignment;
			const usize headerSize = AlignUp(sizeof(MemBlock), eff);
			void* base = static_cast<c8*>(ptr) - headerSize;

			::operator delete(base, std::align_val_t{ eff });
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