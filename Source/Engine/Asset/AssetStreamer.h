#pragma once

#include <Engine/Reflection/Object.h>
#include <Engine/Asset/AssetSource.h>

#include <Runtime/Definitions/Result.h>

namespace CusEngine {
	class Asset;

	class ENGINE_API AssetStreamer : public Object {
		REFLECT_CLASS()
	public:
		virtual ~AssetStreamer() = default;

		virtual Runtime::Result Cook(AssetSource& source) { return Runtime::Result(); }
		virtual Asset* Import(AssetSource& source) { return nullptr; }
	};
}