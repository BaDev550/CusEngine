#include "WidgetRenderPass.h"

#include <Engine/Renderer/RenderSubsystem.h>
#include <Engine/Renderer/RenderPasses/ForwardRenderPass.h>
#include <Engine/Renderer/RenderPasses/SwapchainRenderPass.h>
#include <imgui.h>

namespace Tourqe::Engine {
	WidgetRenderPass::WidgetRenderPass() {
		Depends<ForwardRenderPass>();
		Depends<SwapchainRenderPass>();
	}

	WidgetRenderPass::~WidgetRenderPass() { }

	void WidgetRenderPass::OnInitialized()
	{

	}

	void WidgetRenderPass::Begin(Runtime::RHI::CommandBuffer* cmd) {
		auto* swapchainRenderPass = GetRenderSystem()->GetRenderPass<SwapchainRenderPass>();

		cmd->BeginImGui();
		ImGui::NewFrame();

		Runtime::RHI::RenderingSubmitInfo info = swapchainRenderPass->GetRenderSubmitInfo();
		info.colorAttachments[0].loadOp = RHI::ImageLoadOp::Load;

		cmd->BeginDynamicRendering(info);
	}

	void WidgetRenderPass::End(Runtime::RHI::CommandBuffer* cmd) {
		ImGui::Render();
		cmd->RenderImGui();
		cmd->EndDynamicRendering();
	}
}