#pragma once
#include <Runtime/Definitions/Types.h>
#include <Runtime/RHI/Common/RHIFormat.h>
#include <vector>

namespace Runtime::RHI {
	struct VertexInputDesc {
		std::vector<Format> attribInputs;
		usize stride = 0;

		VertexInputDesc() = default;
		VertexInputDesc(const std::initializer_list<Format>& attribInputs) : attribInputs(attribInputs) {}
	};
}