#pragma once

#include <Runtime/Definitions/Types.h>

namespace Runtime::RHI {
	enum class ImageLoadOp : u8 {
		Load,
		Clear,
		DontCare
	};
}