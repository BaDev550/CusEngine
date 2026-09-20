#pragma once
#include <Engine/Core/Types.h>
#include <Engine/Core/Logger.h>

#include <memory>
#include <unordered_map>

namespace CusEngine::Mem {
	struct MemBlock {
		usize size;
		usize alignment;
	};

	class MemoryTracker final {
	public:
		void Record(void* ptr, MemBlock block) {
			_memoryBlocks[ptr] = block;
		}

		[[nodiscard]] MemBlock Release(void* ptr) {
			auto it = _memoryBlocks.find(ptr);
			if (it != _memoryBlocks.end()) {
				MemBlock block = it->second;
				_memoryBlocks.erase(it);
				return block;
			}
			else {
				Logger::Warn("MemoryTracker", "Tried to free untracked memory at adress: {}", ptr);
				return {};
			}
		}
	private:
		std::unordered_map<void*, MemBlock> _memoryBlocks;
	};

	template<class T>
	using Unique = std::unique_ptr<T>;

	class Allocator final {
	public:
		static void* Allocate(usize size, usize align = 16) {
			void* mem = ::operator new(size, std::align_val_t(align));
			_tracker.Record(mem, { size, align });
			return mem;
		}

		static void Free(void* ptr) {
			MemBlock block = _tracker.Release(ptr);
			::operator delete(ptr, block.alignment);
		}

		template<typename T, typename... Args>
		static T* Construct(Args&&... args) {
			void* mem = Allocate(sizeof(T), alignof(T));
			return new (mem) T(std::forward<Args>(args)...);
		}

		template<typename T>
		static void Destroy(void* ptr) {
			if (ptr) {
				static_cast<T*>(ptr)->~T();
				Free(ptr);
			}
		}

		template<typename T, typename... Args>
		static Unique<T> ConstructUnique(Args&&... args) {
			return std::make_unique<T>(std::forward<Args>(args)...);
		}
	private:
		static inline MemoryTracker _tracker;
	};
}