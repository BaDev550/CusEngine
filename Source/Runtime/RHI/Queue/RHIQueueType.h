#pragma once

#include <Runtime/Definitions/Types.h>

namespace Runtime::RHI {
	enum class QueueType : u8 {
		Graphics = 0,
		Compute,
		Transfer
	};
}