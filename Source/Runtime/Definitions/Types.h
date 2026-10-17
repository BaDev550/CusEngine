#pragma once

#include <cstdint>
#include <limits>
#include <iostream>

#define BIT(x) (1 << x)
#define CORE_DEFINE_ENUM_FLAG_OPERATORS(Enum) \
    inline Enum operator|(Enum lhs, Enum rhs) { \
        return static_cast<Enum>(static_cast<std::underlying_type_t<Enum>>(lhs) | static_cast<std::underlying_type_t<Enum>>(rhs)); \
    } \
    inline Enum operator&(Enum lhs, Enum rhs) { \
        return static_cast<Enum>(static_cast<std::underlying_type_t<Enum>>(lhs) & static_cast<std::underlying_type_t<Enum>>(rhs)); \
    }

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