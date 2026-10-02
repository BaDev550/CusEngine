#pragma once

#include <Runtime/Definitions/Types.h>

namespace Runtime::RHI {
	struct PushConstantRange {
		usize size;
		usize offset;
	};
}