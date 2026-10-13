#include "WidgetRenderPass.h"

#include <Engine/Renderer/RenderSubsystem.h>
#include <Engine/Renderer/RenderPasses/ForwardRenderPass.h>
#include <Engine/Renderer/RenderPasses/SwapchainRenderPass.h>
#include <imgui.h>

namespace Tourqe::Engine {
	WidgetRenderPass::WidgetRenderPass(RenderSubsystem* system) : RenderPass(system) {
		Depends<ForwardRenderPass>();
		Depends<SwapchainRenderPass>();

		_debugInfo.label = GetClassName();
		_debugInfo.color = glm::vec3(1.0f, 1.0f, 1.0f);
	}
	WidgetRenderPass::~WidgetRenderPass() { }

	void WidgetRenderPass::Begin(Runtime::RHI::CommandBuffer* cmd) {
		auto* swapchainRenderPass = GetRenderSystem()->GetRenderPass<SwapchainRenderPass>();

		cmd->BeginImGui();
		ImGui::NewFrame();

		Runtime::RHI::RenderingSubmitInfo info = swapchainRenderPass->GetRenderSubmitInfo();
		for (auto& attachment : info.colorAttachments) {
			attachment.loadOp = RHI::ImageLoadOp::Load;
		}
		cmd->BeginDynamicRendering(info, &_debugInfo);
	}

	void WidgetRenderPass::End(Runtime::RHI::CommandBuffer* cmd) {
		ImGui::Render();
		cmd->RenderImGui();
		cmd->EndDynamicRendering();
	}
}