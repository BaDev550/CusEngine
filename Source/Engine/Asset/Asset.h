#pragma once
#include <Engine/Asset/AssetLoadStat.h>
#include <Engine/Reflection/Object.h>

#include <Runtime/Definitions/UUID.h>

namespace CusEngine {
	class AssetStreamer;
	
	struct AssetHeader {
		u32 magic = ASSET_MAGIC;
		UUID handle = 0;
		c8 typeName[256] = "\0";
		u64 metaSize = 0;
		u64 dataOffset = 0;
		u64 dataSize = 0;
	};

	class ENGINE_API Asset : public Object {
		REFLECT_CLASS();
	public:
		Asset() = default;
		virtual ~Asset() = default;

		[[nodiscard]] const UUID GetAssetHandle() const { return _assetHandle; }
		void SetAssetHandle(UUID handle) { _assetHandle = handle; }

		[[nodiscard]] AssetStreamer* GetAssetStreamer() const { return _assetStreamer; }
		void SetAssetStreamer(AssetStreamer* streamer) { _assetStreamer = streamer; }

		[[nodiscard]] AssetState GetAssetState() const { return _assetState; }
		void SetAssetState(AssetState state) { _assetState = state; }
	private:
		UUID _assetHandle;
		AssetState _assetState = AssetState::InDisk;

		AssetStreamer* _assetStreamer;
	};
}