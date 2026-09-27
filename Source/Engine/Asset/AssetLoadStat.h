#pragma once
#include <Engine/Core/Core.h>

namespace CusEngine {
	enum class AssetState : u8 {
		InDisk,
		Loading,
		Loaded,
		Ready
	};
}