#pragma once
#include <Engine/Core/Core.h>

namespace Tourqe::Engine {
	enum class AssetState : u8 {
		InDisk,
		Loading,
		Loaded,
		Ready
	};
}