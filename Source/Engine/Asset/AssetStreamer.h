#pragma once

#include <Engine/Core/Object.h>
#include <Engine/Core/Result.h>
#include <Engine/Asset/AssetSource.h>

namespace CusEngine {
	class Asset;

	class ENGINE_API AssetStreamer : public Object {
		REFLECT_CLASS()
	public:
		virtual ~AssetStreamer() = default;

		virtual Result Cook(AssetSource& source) { return Result(); }
		virtual Asset* Import(AssetSource& source) { return nullptr; }
	};
}