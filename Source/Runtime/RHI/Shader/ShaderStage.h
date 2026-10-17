#pragma once
#include <Runtime/Definitions/Types.h>

namespace Runtime::RHI {
	enum class ShaderStage : u8 {
		None = 0,
		Vertex = BIT(1),
		Fragment = BIT(2),
		Mesh = BIT(3),
		Compute = BIT(4),
		All = Vertex | Fragment | Mesh | Compute
	};
	CORE_DEFINE_ENUM_FLAG_OPERATORS(ShaderStage);
}
