#pragma once
#include "Core/Core.h"

namespace CusEngine {
	class ENGINE_API Module {
	public:
		virtual ~Module() = default;
		virtual void OnStartup() {}
		virtual void OnDestroy() {}
	};
}