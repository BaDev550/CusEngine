#pragma once

#include <Runtime/Definitions/Types.h>

namespace Runtime::RHI {
	enum class SemaphoreType : u8 {
		Binary = 0,
		Timeline
	};

	struct FenceDesc {
		SemaphoreType type = SemaphoreType::Binary;
		int initialValue = 0;
	};
}