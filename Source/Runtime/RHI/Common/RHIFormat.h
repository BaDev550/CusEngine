#pragma once
#include <Engine/Core/Core.h>

namespace CusEngine::RHI {
	enum class Format : u8 {
		Undefined = 0,
		RG8,
		RGB8,
		RGBA8,
		RGBA16,
		RGBA,

		D32_SFLOAT,
		D16_UNORM,
		D24_UNORM_S8_UINT
	};
}