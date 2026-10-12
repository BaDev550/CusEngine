#pragma once

#include <Engine/Renderer/RenderPass.h>

#include <Runtime/RHI/Image/RHIImage.h>
#include <Runtime/RHI/Common/RHIFormat.h>

namespace Tourqe::Engine {
	TCLASS()
	class ENGINE_API SwapchainRenderPass final : public RenderPass {
		GENERATE_CLASS(SwapchainRenderPass)
	public:
		virtual void Begin(Runtime::RHI::CommandBuffer* cmd) override;
		virtual void End(Runtime::RHI::CommandBuffer* cmd) override;

		[[nodiscard]] Runtime::RHI::Image* GetColorAttachmentImage() const { return _colorAttachmentImage; }
		[[nodiscard]] Runtime::RHI::Image* GetDepthAttachmentImage() const { return _depthAttachmentImage; }
		[[nodiscard]] Runtime::RHI::Format GetColorFormat() const;
		[[nodiscard]] Runtime::RHI::Format GetDepthFormat() const;
		[[nodiscard]] Runtime::RHI::RenderingSubmitInfo GetRenderSubmitInfo() const { return _renderSubmitInfo; }
	private:
		Runtime::RHI::Image* _colorAttachmentImage;
		Runtime::RHI::Image* _depthAttachmentImage;
		Runtime::RHI::RenderingSubmitInfo _renderSubmitInfo;

		glm::vec4 _clearColor = { 0.1f, 0.1f, 0.1f, 1.0f };
	};
}