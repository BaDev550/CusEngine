#pragma once
#include <Engine/Core/Core.h>

namespace Runtime::RHI {
	enum class BufferUsage : u8 {
		None = 0,
		Uniform = BIT(0),
		Storage = BIT(1),
		TransferSrc = BIT(2),
		TransferDst = BIT(3),
		DeviceAddress = BIT(4),
		Index = BIT(5),
		Vertex = BIT(6)
	};
	CORE_DEFINE_ENUM_FLAG_OPERATORS(BufferUsage);
}