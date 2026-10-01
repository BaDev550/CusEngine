#pragma once
#include <Engine/Core/Core.h>

namespace Runtime::RHI {
	struct ShaderDesc {
		void* code = nullptr;
		usize size = 0;
	};
}