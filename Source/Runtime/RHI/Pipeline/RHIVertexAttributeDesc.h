#pragma once
#include <Engine/Core/Core.h>
#include <Runtime/RHI/Common/RHIFormat.h>

namespace Runtime::RHI {
	struct VertexInputAttributeDesc {
		std::vector<Format> inputs;

		VertexInputAttributeDesc() = default;
		VertexInputAttributeDesc(const std::initializer_list<Format>& formatInputs) : inputs(formatInputs) {}
	};
}