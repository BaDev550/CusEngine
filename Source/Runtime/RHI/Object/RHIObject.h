#pragma once
#include <Runtime/Definitions/Logger.h>
#include <string_view>

namespace Runtime::RHI {
	class Context;

	class NonCopyableObject {
	protected:
		NonCopyableObject() = default;
		~NonCopyableObject() = default;
		NonCopyableObject(const NonCopyableObject&) = delete;
		NonCopyableObject& operator=(const NonCopyableObject&) = delete;
		NonCopyableObject(NonCopyableObject&&) = delete;
		NonCopyableObject& operator=(NonCopyableObject&&) = delete;
	};

	class Object : public NonCopyableObject {
	public:
		Object(Context* context) : _context(context) {}
		virtual ~Object() = default;

		virtual void SetObjectDebugName(const char* name) { _debugName = name; }
		std::string_view GetObjectDebugName() const { return _debugName; }
	protected:
		template<class T = Context> requires std::is_base_of_v<Context, T>
		inline T* GetOwningRHIContext() { return static_cast<T*>(_context); }

		Context* _context = nullptr;
		std::string_view _debugName;
	};
}