#pragma once
#include <Engine/Core/Memory.h>

namespace CusEngine::Mem {
	class RefCounted {
	public:
		RefCounted() noexcept = default;
		virtual ~RefCounted() = default;
		RefCounted(const RefCounted&) = delete;
		RefCounted& operator=(const RefCounted&) = delete;

		void AddRef() const noexcept {
			_refCount++;
		}

		void ReleaseRef() const noexcept {
			_refCount--;

			if (_refCount == 0) {
				DestroySelf();
			}
		}

		[[nodiscard]] uint32_t GetRefCount() const noexcept {
			return _refCount.load(std::memory_order_relaxed);
		}

	protected:
		void DestroySelf() const noexcept {
			delete this;
		}
	private:
		mutable std::atomic<u32> _refCount{ 0 };
	};

	template<typename T>
	class Ref {
	public:
		Ref() noexcept : _ptr(nullptr) {}
		Ref(std::nullptr_t) noexcept : _ptr(nullptr) {}

		explicit Ref(T* ptr) noexcept : _ptr(ptr) {
			if (_ptr) {
				_ptr->AddRef();
			}
		}

		~Ref() noexcept {
			Reset();
		}

		Ref(const Ref& other) noexcept : _ptr(other._ptr) {
			if (_ptr) _ptr->AddRef();
		}

		template<typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
		Ref(const Ref<U>& other) noexcept : _ptr(other.Get()) {
			if (_ptr) _ptr->AddRef();
		}

		Ref& operator=(const Ref& other) noexcept {
			if (_ptr != other._ptr) {
				Reset();
				_ptr = other._ptr;
				if (_ptr) _ptr->AddRef();
			}
			return *this;
		}

		template<typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
		Ref& operator=(const Ref<U>& other) noexcept {
			if (_ptr != other.Get()) {
				Reset();
				_ptr = other.Get();
				if (_ptr) _ptr->AddRef();
			}
			return *this;
		}

		Ref(Ref&& other) noexcept : _ptr(other._ptr) {
			other._ptr = nullptr;
		}

		template<typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
		Ref(Ref<U>&& other) noexcept : _ptr(other.Get()) {
			other._ptr = nullptr;
		}

		Ref& operator=(Ref&& other) noexcept {
			if (_ptr != other._ptr) {
				Reset();
				_ptr = other._ptr;
				other._ptr = nullptr;
			}
			return *this;
		}

		template<typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
		Ref& operator=(Ref<U>&& other) noexcept {
			if (_ptr != other.Get()) {
				Reset();
				_ptr = other.Get();
				other._ptr = nullptr;
			}
			return *this;
		}

		void Reset() noexcept {
			if (_ptr) {
				_ptr->ReleaseRef();
				_ptr = nullptr;
			}
		}

		[[nodiscard]] T* Get() const noexcept { return _ptr; }
		T* operator->() const noexcept { return _ptr; }
		T& operator*() const noexcept { return *_ptr; }
		explicit operator bool() const noexcept { return _ptr != nullptr; }

		bool operator==(const Ref& other) const noexcept { return _ptr == other._ptr; }
		bool operator!=(const Ref& other) const noexcept { return _ptr != other._ptr; }

		template<typename... Args>
		[[nodiscard]] static Ref<T> Create(Args&&... args) {
			return Ref<T>(Allocator::Construct<T>(std::forward<Args>(args)...));
		}

		template<typename To, typename From>
		[[nodiscard]] static Ref<To> CastStatic(const Ref<From>& other) noexcept {
			return Ref<To>(static_cast<To*>(other.Get()));
		}

		template<typename TargetType>
		[[nodiscard]] Ref<TargetType> AsStatic() const noexcept {
			return Ref<TargetType>(static_cast<TargetType*>(_ptr));
		}
	private:
		template<typename U> friend class Ref;
		T* _ptr = nullptr;
	};
}