#pragma once
#include <Engine/Core/Core.h>
#include "Engine/Reflection/TypeDescriptor.h"
#include <Engine/Reflection/ReflectionMacros.h>

namespace CusEngine {
    class ENGINE_API Object {
    public:
        virtual ~Object() = default;
        virtual const Reflect::ClassType* GetTypeInfo() const = 0;
    };
}