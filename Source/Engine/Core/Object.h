#pragma once
#include <Engine/Core/Core.h>
#include <string_view>

#include <Runtime/Reflection/ReflectObject.h>

namespace Tourqe::Engine {
	TCLASS()
    class ENGINE_API TObject : public Runtime::Reflection::ReflectObject {
		GENERATE_CLASS(TObject)
    public:
		TObject() = default;
		virtual ~TObject() = default;
		TObject(const TObject&&) = delete;
		TObject& operator=(const TObject&&) = delete;
		TObject(TObject&&) = delete;
		TObject& operator=(TObject&&) = delete;
    };
}