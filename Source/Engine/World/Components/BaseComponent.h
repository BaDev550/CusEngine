#pragma once

#include <Runtime/Definitions/Types.h>
#include <Runtime/Reflection/ReflectObject.h>

namespace Tourqe::Engine {
	TCLASS()
	class BaseComponent : public Runtime::Reflection::ReflectObject {
		GENERATE_CLASS(BaseComponent)
	public:
		BaseComponent() = default;
		virtual ~BaseComponent() = default;
		BaseComponent(const BaseComponent&) = delete;
		BaseComponent& operator=(const BaseComponent&) = delete;
	};
}