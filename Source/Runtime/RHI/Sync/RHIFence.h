#pragma once

#include <Runtime/RHI/Object/RHIObject.h>

namespace Runtime::RHI {
	class Fence : public Object {
	public:
		using Object::Object;
		virtual ~Fence() = default;

		virtual void Wait(u64 value) = 0;
		virtual void Signal(u64 value) = 0;
		virtual u64 GetCurrentValue() = 0;
	};
}