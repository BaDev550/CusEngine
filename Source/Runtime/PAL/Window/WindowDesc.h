#pragma once

#include <Runtime/Definitions/Types.h>
#include <Runtime/PAL/Platform.h>
#include <string>

namespace Runtime::PAL {
	using PALInstance = uptr;
	using WindowHandle = uptr;

	struct WindowDesc {
		u32 width = 800;
		u32 height = 800;
		std::string title = "Window";

		b8 minimized = false;
	};
}