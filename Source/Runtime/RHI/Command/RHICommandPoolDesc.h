#pragma once

#include <Runtime/Definitions/Types.h>

namespace Runtime::RHI {
	enum class CommandPoolUsage : u8 {
		Transient = 0,
		Resettable
	};

	struct CommandPoolDesc {
		u32 queueFamilyIndex = 0;
		CommandPoolUsage usage = CommandPoolUsage::Transient;
	};
}