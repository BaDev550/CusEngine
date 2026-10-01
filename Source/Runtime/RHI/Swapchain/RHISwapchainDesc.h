#pragma once
#include <Engine/Core/Core.h>

namespace Runtime::RHI {
	struct SwapchainDesc {
		u32 width;
		u32 height;
		bool vsync = true;
	};
}