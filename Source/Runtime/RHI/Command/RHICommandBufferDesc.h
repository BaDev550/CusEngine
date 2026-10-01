#pragma once

#include <Runtime/Definitions/Types.h>

namespace Runtime::RHI {
	enum class CommandBufferType : u8 {
		Primary = 0,
		Secondary
	};

	struct CommandBufferDesc {
		CommandBufferType type = CommandBufferType::Primary;
	};
}