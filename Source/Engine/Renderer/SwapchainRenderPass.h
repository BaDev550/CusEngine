#pragma once

#include <Engine/Renderer/RenderPass.h>

namespace Tourqe::Engine {
	TCLASS()
	class ENGINE_API SwapchainRenderPass final : public RenderPass {
		GENERATE_CLASS(SwapchainRenderPass)
	public:
		virtual void Begin() override {}
		virtual void End() override {}

		[[nodiscard]] Runtime::RHI::Image* GetDepthAttachmentImage() const { return _depthAttachmentImage; }
	private:
		Runtime::RHI::Image* _colorAttachmentImage;
		Runtime::RHI::Image* _depthAttachmentImage;
	};
}