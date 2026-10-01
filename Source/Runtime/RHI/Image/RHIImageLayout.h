#pragma once
#include <Engine/Core/Core.h>

namespace Runtime::RHI {
	enum class ImageLayout : u8 {
		Undefined = 0,
		PresentSrc,
		TransferDst,
		TransferSrc,
		ColorAttachment,
		DepthAttachment,
		ShaderReadOnly
	};
}