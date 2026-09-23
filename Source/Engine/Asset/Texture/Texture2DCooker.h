#pragma once

#include <Engine/Asset/AssetCooker.h>

namespace CusEngine {
	class Texture2DCooker final : AssetCooker {
	public:
		virtual Result Cook(AssetSource& source) override;
	};
}