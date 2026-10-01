#pragma once

#include <Runtime/RHI/Object/RHIObject.h>

namespace Runtime::RHI {
	class Fence : public Object {
	public:
		virtual ~Fence() = default;

		virtual void Wait(u64 value) = 0;
	};
}