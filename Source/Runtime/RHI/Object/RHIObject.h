#pragma once
#include <Engine/Core/Logger.h>
#include <string_view>

namespace CusEngine::RHI {
	class Context;

	class RHIObject {
	public:
		virtual ~RHIObject() = default;
		virtual void SetObjectDebugName(const char* name) { }
		virtual std::string_view GetObjectDebugName() const { return "rhi_object_unknown"; }

		template<class T = Context>
		T* GetContext() { return static_cast<T*>(_context); }

		Context* _context = nullptr;
	};
}