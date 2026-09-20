#pragma once
#include "Types.h"

#ifdef _WIN32
#ifdef ENGINE_EXPORTS
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllimport)
#endif
#else
#define ENGINE_API
#endif

#define BIT(x) (1 << x)
#define CORE_DEFINE_ENUM_FLAG_OPERATORS(Enum) \
    inline Enum operator|(Enum lhs, Enum rhs) { \
        return static_cast<Enum>(static_cast<std::underlying_type_t<Enum>>(lhs) | static_cast<std::underlying_type_t<Enum>>(rhs)); \
    } \
    inline Enum operator&(Enum lhs, Enum rhs) { \
        return static_cast<Enum>(static_cast<std::underlying_type_t<Enum>>(lhs) & static_cast<std::underlying_type_t<Enum>>(rhs)); \
    }