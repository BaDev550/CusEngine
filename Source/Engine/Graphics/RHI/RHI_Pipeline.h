#pragma once

#include "Graphics/RHI/RHI.h"
#include "Graphics/RenderObject.h"

namespace CusEngine::RHI {
	class ENGINE_API Pipeline : public RenderObject {
	public:
		Pipeline(RenderCommands* commands) : RenderObject(commands) {}
		Pipeline() : RenderObject(nullptr) {}
		virtual ~Pipeline() = default;

		[[nodiscard]] virtual std::string_view GetObjectDebugName() const override = 0;
		[[nodiscard]] virtual const PipelineDesc& GetDesc() const = 0;
	};
}