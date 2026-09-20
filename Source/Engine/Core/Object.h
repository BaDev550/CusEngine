#pragma once
#include "Core/Core.h"
#include "Reflection/TypeDescriptor.h"

namespace CusEngine {
    class ENGINE_API Object {
    public:
        virtual ~Object() = default;
        virtual const Reflect::ClassType* GetTypeInfo() const = 0;
    };
}