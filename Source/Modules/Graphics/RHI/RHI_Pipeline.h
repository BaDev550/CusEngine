#pragma once

#include "RHI.h"
#include "RHI_Object.h"

namespace Graphics {
	class ENGINE_API RHI_Pipeline : public RHI_Object {
	public:
		RHI_Pipeline(RHI_RenderCommands* commands) : RHI_Object(commands) {}
		RHI_Pipeline() : RHI_Object(nullptr) {}
		virtual ~RHI_Pipeline() = default;

		[[nodiscard]] virtual RHI_PipelineHandle GetNativeHandle() const = 0;
		[[nodiscard]] virtual std::string_view GetObjectDebugName() const override = 0;
		[[nodiscard]] virtual const RHI_PipelineDesc& GetDesc() const = 0;
	};
}