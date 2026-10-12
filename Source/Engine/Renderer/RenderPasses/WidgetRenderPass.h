#pragma once

#include <Engine/Renderer/RenderPass.h>

namespace Tourqe::Engine {
	TCLASS()
	class WidgetRenderPass final : public RenderPass {
	GENERATE_CLASS(WidgetRenderPass)
	public:
		WidgetRenderPass();
		~WidgetRenderPass();

		virtual void OnInitialized() override;

		virtual void Begin(Runtime::RHI::CommandBuffer* cmd) override;
		virtual void End(Runtime::RHI::CommandBuffer* cmd) override;
	private:
		// TODO(0x): idk move imgui context here?
	};
}