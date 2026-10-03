#pragma once

#include <Engine/Asset/AssetSource.h>
#include <Engine/Core/Object.h>

#include <Runtime/Definitions/Result.h>

namespace CusEngine {
	class Asset;

	class ENGINE_API AssetStreamer : public CObject {
		GENERATE_CLASS(AssetStreamer)
	public:
		virtual ~AssetStreamer() = default;

		virtual Runtime::Result Cook(AssetSource& source) { return Runtime::Result(); }
		virtual Asset* Import(AssetSource& source) { return nullptr; }

		virtual std::string GetAssetClassName() { return "Asset"; };
	};
}