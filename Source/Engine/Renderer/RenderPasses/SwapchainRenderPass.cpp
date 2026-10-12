#include "SwapchainRenderPass.h"
#include <Engine/Core/Engine.h>
#include <Engine/Renderer/RenderSubsystem.h>

namespace Tourqe::Engine {
	void SwapchainRenderPass::Begin(Runtime::RHI::CommandBuffer* cmd) {
		auto* swapchain = GetRenderSystem()->GetSwapchain();
		u32 imageIndex = GetRenderSystem()->GetImageIndex();

		_colorAttachmentImage = swapchain->GetColorAttachments()[imageIndex];
		_depthAttachmentImage = swapchain->GetDepthAttachment();

		RHI::ColorAttachment colorAttachment{};
		colorAttachment.image = _colorAttachmentImage;
		colorAttachment.clearColor = _clearColor;

		_renderSubmitInfo.colorAttachments = { _colorAttachmentImage };
		_renderSubmitInfo.depthAttachment = _depthAttachmentImage;
		_renderSubmitInfo.extent = swapchain->GetExtent();

		cmd->BeginDynamicRendering(_renderSubmitInfo);
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