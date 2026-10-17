#pragma once

#include <Runtime/Reflection/ReflectObject.h>
#include <Runtime/Reflection/Macros.h>

namespace Tourqe::Engine {
	TCLASS();
	class Widget : public Runtime::Reflection::ReflectObject {
		GENERATE_CLASS(Widget);
	public:
		virtual ~Widget() = default;

		virtual void Draw() = 0;
	};
}