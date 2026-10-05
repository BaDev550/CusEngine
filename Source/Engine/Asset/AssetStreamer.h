#pragma once

#include <Engine/Asset/AssetSource.h>

#include <Runtime/Reflection/Type.h>
#include <Runtime/Definitions/Result.h>
#include <Runtime/Reflection/ReflectionMacros.h>

namespace CusEngine {
	class Asset;

	class ENGINE_API AssetStreamer : public Runtime::Reflection::Type {
		GENERATE_CLASS(AssetStreamer)
	public:
		virtual ~AssetStreamer() = default;

		virtual Runtime::Result Cook(AssetSource& source) { return Runtime::Result(); }
		virtual Asset* Import(AssetSource& source) { return nullptr; }

		virtual std::string GetAssetClassName() { return "Asset"; };
	};
}