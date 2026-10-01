#pragma once

#include <Runtime/RHI/Pipeline/RHIPipelineDesc.h>
#include <Runtime/RHI/Object/RHIObject.h>

namespace Runtime::RHI {
	class ENGINE_API Pipeline : public Object {
	public:
		virtual ~Pipeline() = default;

		[[nodiscard]] virtual const PipelineDesc& GetDesc() const = 0;
	};
}