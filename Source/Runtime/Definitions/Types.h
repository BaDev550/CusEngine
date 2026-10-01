#pragma once

#include <cstdint>
#include <limits>

using u8 =	uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using i8 =	int8_t;
using i16 = int16_t;
using i32 = int32_t;

using uptr = uintptr_t;
using usize = size_t;
using b8 = bool;

using c8 = char;
using c16 = wchar_t;

inline constexpr u32 u32_max = std::numeric_limits<u32>::max();