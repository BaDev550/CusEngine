#pragma once

#include <Runtime/Definitions/Types.h>

namespace Tourqe::Engine {
	struct RenderGraphHandle {
		u32 id = ~0u;

		[[nodiscard]] bool IsValid() const { return id != ~0u; }
	};
}