#pragma once
#include <Engine/Core/Object.h>
#include <Engine/Core/UUID.h>

namespace CusEngine {
	class AssetStreamer;

#define ASSET_EXTENSION ".casset"
	struct AssetHeader {
		u32 magic;
		u32 width;
		u32 height;
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

		[[nodiscard]] const UUID GetAssetID() const { return _id; }
		void SetAssetID(UUID id) { _id = id; }

		[[nodiscard]] AssetStreamer* GetAssetStreamer() const { return _streamer; }
		void SetAssetStreamer(AssetStreamer* streamer) { _streamer = streamer; }
	private:
		UUID _id;
		AssetStreamer* _streamer;
	};
}