#pragma once

#include <Engine/Renderer/RenderPass.h>

#include <Runtime/RHI/Image/RHIImage.h>
#include <Runtime/RHI/Common/RHIFormat.h>

namespace Tourqe::Engine {
	TCLASS()
	class ENGINE_API SwapchainRenderPass final : public RenderPass {
		GENERATE_CLASS(SwapchainRenderPass)
	public:
		SwapchainRenderPass(RenderSubsystem* system);

		virtual void Begin(Runtime::RHI::CommandBuffer* cmd) override;
		virtual void End(Runtime::RHI::CommandBuffer* cmd) override;

		[[nodiscard]] Runtime::RHI::Image* GetColorAttachmentImage() const { return _colorAttachmentImage; }
		[[nodiscard]] Runtime::RHI::Image* GetDepthAttachmentImage() const { return _depthAttachmentImage; }
		[[nodiscard]] Runtime::RHI::Format GetColorFormat() const;
		[[nodiscard]] Runtime::RHI::Format GetDepthFormat() const;
	private:
		Runtime::RHI::Image* _colorAttachmentImage;
		Runtime::RHI::Image* _depthAttachmentImage;

		glm::vec4 _clearColor = { 0.1f, 0.1f, 0.1f, 1.0f };
	};
}