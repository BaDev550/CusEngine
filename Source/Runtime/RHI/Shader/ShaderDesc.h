#pragma once
#include <Engine/Core/Core.h>

namespace CusEngine::RHI {
	struct ShaderDesc {
		void* code = nullptr;
		usize size = 0;
	};
}