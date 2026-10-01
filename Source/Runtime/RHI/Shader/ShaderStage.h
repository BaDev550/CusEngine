#pragma once
#include <Engine/Core/Core.h>

namespace Runtime::RHI {
	enum class ShaderStage : u8 {
		Vertex,
		Fragment,
		Mesh,
		Compute
	};
}
