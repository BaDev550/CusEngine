#pragma once
#include <Engine/Core/Core.h>

namespace CusEngine::RHI {
	enum class ShaderStage : u8 {
		Vertex,
		Fragment,
		Mesh,
		Compute
	};
}
