#pragma once

#include <Engine/Core/Engine.h>
#include <Engine/Reflection/ReflectionSubsystem.h>

#include <vector>
#include <utility>

#define CCLASS(...)
#define CPROP(...)

namespace CusEngine::Reflect {
	struct ClassType;
	ENGINE_API std::vector<ClassType>& GetPendingClasses();
}

#define GENERATE_CLASS(className) \
public: \
    static constexpr std::string_view StaticClassName() { return #className; } \
    virtual const CusEngine::Reflect::ClassType* GetClassInfo() const override { return nullptr; }