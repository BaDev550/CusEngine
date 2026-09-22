#pragma once
#include <Engine/Core/Object.h>
#include <Engine/Core/UUID.h>

namespace CusEngine {
	struct AssetData {
		constexpr static u32 Magic = 0x41534554; // ASET

		u32 magic;
		UUID id;
		char typeName[256];
		u64 metaSize = 0;
		u64 dataOffset = 0;
		u64 dataSize = 0;
	};

	CUS_CLASS();
	class ENGINE_API Asset : public Object {
	public:
		Asset() = default;
		virtual ~Asset() = default;
	};
}