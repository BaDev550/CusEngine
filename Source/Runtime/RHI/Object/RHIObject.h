#pragma once
#include <Engine/Core/Logger.h>
#include <string_view>

namespace CusEngine::RHI {
	class Context;

	class Object {
	public:
		Object(Context* context) : _context(context) {}
		virtual ~Object() = default;
		Object(const Object&) = delete;
		Object& operator=(const Object&) = delete;

		virtual void SetObjectDebugName(const char* name) { _debugName = name; }
		std::string_view GetObjectDebugName() const { return _debugName; }
	protected:
		template<class T = Context> requires std::is_base_of_v<Context, T>
		inline T* GetOwningRHIContext() { return static_cast<T*>(_context); }

		Context* _context = nullptr;
		std::string_view _debugName;
	};
}