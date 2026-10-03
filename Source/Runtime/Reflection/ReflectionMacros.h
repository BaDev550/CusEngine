#pragma once

#include <Engine/Core/Engine.h>

#include <vector>
#include <utility>

#define CCLASS(...)
#define CPROP(...)

#define GENERATE_CLASS(className) \
public: \
    static constexpr std::string_view StaticClassName() { return #className; }