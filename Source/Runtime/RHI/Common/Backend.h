#pragma once
#include <Engine/Core/Core.h>

namespace CusEngine::RHI {
	enum class GraphicsBackend : u8 {
		None = 0,
		Vulkan = 1,
		OpenGL = 2
	};
}