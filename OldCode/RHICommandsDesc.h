#pragma once
#include <Engine/Core/Core.h>

namespace Runtime::RHI {
	class Swapchain;

	struct CommandsDesc {
		Swapchain* targetSwapchain;
	};
}