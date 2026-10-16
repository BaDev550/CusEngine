#include "SwapchainRenderPass.h"
#include <Engine/Core/Engine.h>
#include <Engine/Renderer/RenderSubsystem.h>

namespace Tourqe::Engine {
	SwapchainRenderPass::SwapchainRenderPass(RenderSubsystem* system) : RenderPass(system) {
		_debugInfo.label = GetClassName();
		_debugInfo.color = glm::vec3(1.0f, 0.0f, 0.0f);
	}

	void SwapchainRenderPass::Begin(Runtime::RHI::CommandBuffer* cmd) {
		auto* swapchain = GetRenderSystem()->GetSwapchain();
		u32 imageIndex = GetRenderSystem()->GetImageIndex();

		_colorAttachmentImage = swapchain->GetColorAttachments()[imageIndex];
		_depthAttachmentImage = swapchain->GetDepthAttachment();
		cmd->TransitionImageLayout(_colorAttachmentImage, RHI::ImageLayout::ColorAttachment);

		RHI::ColorAttachment colorAttachment{};
		colorAttachment.image = _colorAttachmentImage;
		colorAttachment.clearColor = _clearColor;

		_renderSubmitInfo.colorAttachments = { _colorAttachmentImage };
		_renderSubmitInfo.depthAttachment = _depthAttachmentImage;
		_renderSubmitInfo.extent = swapchain->GetExtent();

		cmd->BeginDynamicRendering(_renderSubmitInfo, &_debugInfo);
	}

	void SwapchainRenderPass::End(Runtime::RHI::CommandBuffer* cmd) {
		cmd->EndDynamicRendering();
	}

	Runtime::RHI::Format SwapchainRenderPass::GetColorFormat() const {
		auto* swapchain = GetRenderSystem()->GetSwapchain();
		return swapchain->GetColorFormat();
	}

	Runtime::RHI::Format SwapchainRenderPass::GetDepthFormat() const {
		auto* swapchain = GetRenderSystem()->GetSwapchain();
		return swapchain->GetDepthFormat();
	}
}