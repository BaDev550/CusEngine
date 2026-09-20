#pragma once

#include <Runtime/RHI/Buffer/RHIBufferUsage.h>
#include <Runtime/RHI/Buffer/RHIMemoryUsage.h>

namespace CusEngine::RHI {
	struct BufferDesc {
		usize size = 0;
		BufferUsage usage = BufferUsage::None;
		MemoryUsage memoryUsage = MemoryUsage::CPU;
		AllocationFlagBits allocationFlags = AllocationFlagBits::None;
	};
}