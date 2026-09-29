#pragma once
#include <Engine/Asset/AssetLoadStat.h>
#include <Engine/Core/Object.h>
#include <Engine/Core/UUID.h>

namespace CusEngine {
	class AssetStreamer;

#define ASSET_EXTENSION ".casset"
	struct AssetHeader {
		u32 magic;
		UUID id;
		char typeName[256];
		u64 metaSize = 0;
		u64 dataOffset = 0;
		u64 dataSize = 0;
	};

	class ENGINE_API Asset : public Object {
		REFLECT_CLASS();
	public:
		constexpr static u32 Magic = 0x41534554; // ASET

		Asset() = default;
		virtual ~Asset() = default;

		[[nodiscard]] const UUID GetAssetID() const { return _assetId; }
		void SetAssetID(UUID id) { _assetId = id; }

		[[nodiscard]] AssetStreamer* GetAssetStreamer() const { return _assetStreamer; }
		void SetAssetStreamer(AssetStreamer* streamer) { _assetStreamer = streamer; }

		[[nodiscard]] AssetState GetAssetState() const { return _assetState; }
		void SetAssetState(AssetState state) { _assetState = state; }
	private:
		UUID _assetId;		
		AssetState _assetState = AssetState::InDisk;

		AssetStreamer* _assetStreamer;
	};
}