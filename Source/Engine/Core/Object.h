#pragma once
#include <Engine/Core/Core.h>
#include <string_view>

#include <Runtime/Reflection/Type.h>
#include <Runtime/Reflection/ReflectionMacros.h>

namespace CusEngine {
	CCLASS()
    class ENGINE_API CObject : public Runtime::Reflection::Type {
		GENERATE_CLASS(CObject)
    public:
		CObject() = default;
		virtual ~CObject() = default;
		CObject(const CObject&&) = delete;
		CObject& operator=(const CObject&&) = delete;
		CObject(CObject&&) = delete;
		CObject& operator=(CObject&&) = delete;
    };
}