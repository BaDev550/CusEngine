#pragma once

#include <Runtime/RHI/Pipeline/RHIPipelineDesc.h>
#include <Runtime/RHI/Object/RHIObject.h>

namespace CusEngine::RHI {
	class ENGINE_API Pipeline : public RHIObject {
	public:
		virtual ~Pipeline() = default;

		[[nodiscard]] virtual std::string_view GetObjectDebugName() const override = 0;
		[[nodiscard]] virtual const PipelineDesc& GetDesc() const = 0;
	};
}