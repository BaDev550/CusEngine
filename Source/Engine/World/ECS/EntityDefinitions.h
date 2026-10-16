#pragma once

#include <Runtime/Definitions/Types.h>
#include <Runtime/Definitions/UUID.h>
#include <bitset>

namespace Tourqe::Engine {
	static constexpr u32 MaxEntities = 2048;
	static constexpr u32 MaxComponents = 256;
	
	struct EntityHandle {
		u32 index = 0;
		u32 generation = 0;

		b8 IsValid() const { return index != 0xFFFFFFFF; }
	};

	using Signature = std::bitset<MaxComponents>;
}