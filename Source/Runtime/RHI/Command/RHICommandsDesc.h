#pragma once
#include <Engine/Core/Core.h>

namespace CusEngine::RHI {
	class Swapchain;

	struct CommandsDesc {
		Swapchain* targetSwapchain;
	};
}