#pragma once
#include <Engine/Core/Core.h>

namespace CusEngine::RHI {
	enum class ImageTileMode : u8 {
		Undefined = 0,
		Repeat,
		Mirror,
		ClampToEdge,
		ClampToBorder,
		Optimal
	};
}