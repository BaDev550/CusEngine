#pragma once
#include <Runtime/Definitions/Types.h>

namespace Runtime::RHI {
	enum class ShaderStage : u8 {
		Vertex = 0,
		Fragment = BIT(1),
		Mesh = BIT(2),
		Compute = BIT(3),
		All = Vertex | Fragment | Mesh | Compute
	};
	CORE_DEFINE_ENUM_FLAG_OPERATORS(ShaderStage);
}
