#include "ForwardRenderPass.h"

#include <Engine/Core/Engine.h>
#include <Engine/Renderer/RenderSubsystem.h>
#include <Engine/Renderer/RenderPasses/SwapchainRenderPass.h>
#include <Engine/Asset/AssetSubsystem.h>

namespace Tourqe::Engine {
	ForwardRenderPass::ForwardRenderPass(RenderSubsystem* system) : RenderPass(system) {
		Depends<SwapchainRenderPass>();

		auto assetSystem = Engine::Get()->GetSubsystem<AssetSubsystem>();
		auto swapchainRenderPass = GetRenderSystem()->GetRenderPass<SwapchainRenderPass>();
		{
			Shader* forwardPassVertexShader = assetSystem->Get<Shader>("base_forward_vert.vert");
			Shader* forwardPassFragmentShader = assetSystem->Get<Shader>("base_forward_frag.frag");

			Runtime::RHI::PushConstantRange pcRange{};
			pcRange.size = sizeof(SpritePushConstant);
			pcRange.offset = 0;

			Runtime::RHI::PipelineDesc vertexDesc{};
			vertexDesc.colorFormats = { swapchainRenderPass->GetColorFormat() };
			vertexDesc.depthFormat = swapchainRenderPass->GetDepthFormat();
			vertexDesc.vertexShader = &forwardPassVertexShader->GetDesc();
			vertexDesc.fragmentShader = &forwardPassFragmentShader->GetDesc();
			vertexDesc.pushConstantRanges = { pcRange };
			vertexDesc.blending = false;
			vertexDesc.depthTest = false;

			_forwardPassPipeline = GetRenderSystem()->GetContext()->CreatePipeline(vertexDesc);
		}
	}

	ForwardRenderPass::~ForwardRenderPass() {
		Runtime::Mem::Allocator::Destroy<Runtime::RHI::Pipeline>(_forwardPassPipeline);
	}

	void ForwardRenderPass::Begin(Runtime::RHI::CommandBuffer* cmd) {

	}

	void ForwardRenderPass::End(Runtime::RHI::CommandBuffer* cmd) {
		
	}
}