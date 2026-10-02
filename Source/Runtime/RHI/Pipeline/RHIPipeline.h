#pragma once

#include <Runtime/RHI/Pipeline/RHIPipelineDesc.h>
#include <Runtime/RHI/Object/RHIObject.h>

namespace Runtime::RHI {
	class CommandBuffer;

	class Pipeline : public Object {
	public:
		using Object::Object;
		virtual ~Pipeline() = default;

		virtual void Bind(CommandBuffer* cmd) = 0;
		virtual void PushConstant(CommandBuffer* cmd, void* data, usize size, usize offset = 0) = 0;

		[[nodiscard]] virtual const PipelineDesc& GetDesc() const = 0;
	};
}