#include "RenderSubsystem.h"
#include "Core/Engine.h"

#include "Window/WindowSubsystem.h"
#include "MT/JobSubsystem.h"

namespace CusEngine {
	bool RenderSubsystem::OnCreate(Engine* engine) {
		Subsystem::OnCreate(engine);

		auto window = engine->GetSubsystem<WindowSubsystem>()->GetWindow();

		_commands = RHI::CreateRenderCommands(window->GetRenderContext(), window->GetSwapchain());

		return true;
	}

	void RenderSubsystem::OnUpdate() {
		_commands->BeginFrame();
		BeginSwapchainPass();

		EndSwapchainPass();
		_commands->EndFrame();
	}

	void RenderSubsystem::OnDestroy() {
	
	}

	void RenderSubsystem::BeginSwapchainPass() {
		RHI::Swapchain* swapchain = _commands->GetTargetSwapchain();
		if (!swapchain) return;

		u32 imageIndex = _commands->GetImageIndex();
		const Mem::Ref<RHI::Image>& colorAttachment = swapchain->GetColorAttachments()[imageIndex];
		const Mem::Ref<RHI::Image>& depthAttachment = swapchain->GetDepthAttachment();

		_commands->BeginDynamicRendering({ colorAttachment }, depthAttachment, swapchain->GetExtent());
	}

	void RenderSubsystem::EndSwapchainPass() {
		RHI::Swapchain* swapchain = _commands->GetTargetSwapchain();
		if (!swapchain) return;

		u32 imageIndex = _commands->GetImageIndex();
		const Mem::Ref<RHI::Image>& colorAttachment = swapchain->GetColorAttachments()[imageIndex];

		_commands->EndDynamicRendering();
		_commands->TransitionImageLayout(colorAttachment.Get(), RHI::ImageLayout::PresentSrc);
	}

	void RenderSubsystem::GetDependencyGraph(DependencyGraph & graph) {
		graph.Require<WindowSubsystem>(DependencyOrder::After);
		graph.Require<MT::JobSubsystem>(DependencyOrder::After);
	}
}