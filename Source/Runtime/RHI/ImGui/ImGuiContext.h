#pragma once

#include <Runtime/Definitions/Types.h>
#include <Runtime/RHI/Command/RHICommandBuffer.h>

namespace Runtime::RHI {
	class ImGuiContext {
	public:
		ImGuiContext() = default;
		virtual ~ImGuiContext() = default;
		ImGuiContext(const ImGuiContext&) = delete;
		ImGuiContext& operator=(const ImGuiContext&) = delete;

		virtual void Begin() const = 0;
		virtual void Flush(Runtime::RHI::CommandBuffer* cmd) const = 0;
	};
}