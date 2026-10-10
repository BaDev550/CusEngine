#include "SceneRenderer2D.h"

#include <Engine/Asset/AssetSubsystem.h>
#include <Engine/Window/WindowSubsystem.h>
#include <Engine/Renderer/RenderSubsystem.h>

#include <imgui.h>

namespace Tourqe::Engine {
	Runtime::Result SceneRenderer2DSystem::OnCreate() {
		auto assetSystem = _engine->GetSubsystem<AssetSubsystem>();
		auto renderSystem = _engine->GetSubsystem<RenderSubsystem>();
		{
			Shader* forwardPassVertexShader = assetSystem->Get<Shader>("base_forward_vert.vert");
			Shader* forwardPassFragmentShader = assetSystem->Get<Shader>("base_forward_frag.frag");

			Runtime::RHI::PushConstantRange pcRange{};
			pcRange.size = sizeof(SpritePushConstant);
			pcRange.offset = 0;

			Runtime::RHI::PipelineDesc vertexDesc{};
			vertexDesc.colorFormats = { renderSystem->GetSwapchain()->GetColorFormat()};
			vertexDesc.depthFormat = renderSystem->GetSwapchain()->GetDepthFormat();
			vertexDesc.vertexShader = &forwardPassVertexShader->GetDesc();
			vertexDesc.fragmentShader = &forwardPassFragmentShader->GetDesc();
			vertexDesc.pushConstantRanges = { pcRange };
			vertexDesc.blending = false;
			vertexDesc.depthTest = false;

			_forwardPassPipeline = renderSystem->GetContext()->CreatePipeline(vertexDesc);
		}
		return Runtime::Result();
	}

	void SceneRenderer2DSystem::OnUpdate(Scene& scene) {
		auto renderSystem = _engine->GetSubsystem<RenderSubsystem>();

		renderSystem->Pass([=](Runtime::RHI::CommandBuffer* cmd) {
			auto context = renderSystem->GetContext();
			auto swapchain = renderSystem->GetSwapchain();


			});
	}

	void SceneRenderer2DSystem::OnDestroy() {
		auto renderSystem = _engine->GetSubsystem<RenderSubsystem>();
		renderSystem->GetContext()->WaitDeviceIdle();

		Runtime::Mem::Allocator::Destroy<Runtime::RHI::Pipeline>(_forwardPassPipeline);
	}
}