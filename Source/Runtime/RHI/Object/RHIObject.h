#pragma once
#include <string_view>

namespace CusEngine::RHI {
	class Context;

	class RHIObject {
	public:
		virtual ~RHIObject() = default;
		virtual std::string_view GetObjectDebugName() const = 0;

		template<class T = Context> requires(std::is_base_of_v<Context, T>())
		T* GetContext() { return static_cast<T*>(_context); }
	protected:
		Context* _context = nullptr;
	};
}