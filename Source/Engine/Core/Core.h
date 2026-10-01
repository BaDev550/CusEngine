#pragma once
#include <Runtime/Definitions/Types.h>
#include <iostream>

#ifdef _WIN32
#ifdef ENGINE_EXPORTS
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllimport)
#endif
#endif

#define ASSET_MAGIC 0x41534554 // ASET
#define SHADER_MAGIC 0x53484452 // SHDR
#define TEXTURE2D_MAGIC 0x54585432 // TXT2

#define COLOR_WHITE 0xFFFFFFFF
#define COLOR_BLACK 0x000000FF
#define COLOR_RED 0xFF0000FF
#define COLOR_GREEN 0x00FF00FF
#define COLOR_BLUE 0x0000FFFF

#define CACHE_DIR "cache/"
#define ASSET_EXTENSION ".casset"
#define ASSET_REGISTRY_PATH CACHE_DIR "assetReg.json"

#define BIT(x) (1 << x)
#define CORE_DEFINE_ENUM_FLAG_OPERATORS(Enum) \
    inline Enum operator|(Enum lhs, Enum rhs) { \
        return static_cast<Enum>(static_cast<std::underlying_type_t<Enum>>(lhs) | static_cast<std::underlying_type_t<Enum>>(rhs)); \
    } \
    inline Enum operator&(Enum lhs, Enum rhs) { \
        return static_cast<Enum>(static_cast<std::underlying_type_t<Enum>>(lhs) & static_cast<std::underlying_type_t<Enum>>(rhs)); \
    }