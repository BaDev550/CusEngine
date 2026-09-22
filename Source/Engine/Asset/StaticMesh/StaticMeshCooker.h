#pragma once

#include <Engine/Asset/AssetCooker.h>

namespace CusEngine {
	class ENGINE_API StaticMeshCooker final : AssetCooker {
	public:
		virtual Result Cook(AssetSource& source) override;
	};
}