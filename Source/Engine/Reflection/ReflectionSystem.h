#pragma once

#include <Runtime/Reflection/Type.h>
#include <vector>

namespace Tourqe::Engine {
	class ReflectionSystem final {
	public:
		ReflectionSystem();
		~ReflectionSystem();
	private:
		std::vector<Runtime::Reflection::Type> _types;
	};
}