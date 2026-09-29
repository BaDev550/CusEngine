#pragma once

#include <Engine/Core/Core.h>

namespace CusEngine::RHI {
	enum class ImageFilter : u8 {
		Nearest,
		Linear,
		BiLinear
	};
}