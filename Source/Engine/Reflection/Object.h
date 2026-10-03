#pragma once
#include <Engine/Core/Core.h>
#include "Engine/Reflection/TypeDescriptor.h"
#include <Engine/Reflection/ReflectionMacros.h>

namespace CusEngine {
    class ENGINE_API CObject {
    public:
		CObject() = default;
		virtual ~CObject() = default;
		CObject(const CObject&&) = delete;
		CObject& operator=(const CObject&&) = delete;
		CObject(CObject&&) = delete;
		CObject& operator=(CObject&&) = delete;

        virtual const Reflect::ClassType* GetClassInfo() const = 0;
    };
}