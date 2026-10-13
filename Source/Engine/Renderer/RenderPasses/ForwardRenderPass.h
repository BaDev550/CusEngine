#pragma once

#include <Engine/Renderer/RenderPass.h>

namespace Tourqe::Engine {
	TCLASS()
	class ForwardRenderPass final : public RenderPass {
		GENERATE_CLASS(ForwardRenderPass)
	public:
		ForwardRenderPass(RenderSubsystem* system);
		~ForwardRenderPass();

		virtual void Begin(Runtime::RHI::CommandBuffer* cmd) override;
		virtual void End(Runtime::RHI::CommandBuffer* cmd) override;
	private:
		struct SpritePushConstant {
			u32 textureID;
			u32 samplerID;
		};

		Runtime::RHI::Pipeline* _forwardPassPipeline = nullptr;
	};
}