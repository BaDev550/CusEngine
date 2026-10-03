#pragma once
#include <Engine/Core/Core.h>
#include <Runtime/Reflection/ReflectionMacros.h>

namespace CusEngine {
    class ENGINE_API CObject {
    public:
		CObject() = default;
		virtual ~CObject() = default;
		CObject(const CObject&&) = delete;
		CObject& operator=(const CObject&&) = delete;
		CObject(CObject&&) = delete;
		CObject& operator=(CObject&&) = delete;
    };
}