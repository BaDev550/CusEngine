#include "SceneRenderer2D.h"

#include <Engine/Asset/AssetSubsystem.h>
#include <Engine/Asset/Texture/Texture2D.h>

#include <imgui.h>

namespace CusEngine {
	Runtime::Result SceneRenderer2DSubsystem::OnCreate(Engine* engine) {
		Subsystem::OnCreate(engine);

		auto assetSystem = engine->GetSubsystem<AssetSubsystem>();
		auto renderSystem = engine->GetSubsystem<RenderSubsystem>();
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

		_testSprite = assetSystem->Get<Texture2D>("guven-catak.jpg");

		return Runtime::Result();
	}

	void SceneRenderer2DSubsystem::OnUpdate() {
		auto renderSystem = _engine->GetSubsystem<RenderSubsystem>();

		renderSystem->BeginFrame();

		Runtime::RHI::CommandBuffer* cmd = renderSystem->GetCurrentFrameData()->commandBuffer;
		renderSystem->BeginImGuiPass();

		renderSystem->BeginSwapchainPass();

		if (_testSprite->GetAssetState() == AssetState::Ready) {
			SpritePushConstant pc;
			pc.textureID = _testSprite->_image->GetBindlessIndex();
			pc.samplerID = _testSprite->_image->GetSamplerIndex();

			_forwardPassPipeline->Bind(cmd);
			_forwardPassPipeline->PushConstant(cmd, &pc, sizeof(SpritePushConstant), 0);
			cmd->DrawVertex(_forwardPassPipeline, 6);
		}

		renderSystem->EndImGuiPass();
		renderSystem->EndSwapchainPass();
		renderSystem->EndFrame();
	}

	void SceneRenderer2DSubsystem::OnDestroy() {
		Subsystem::OnDestroy();

		auto renderSystem = _engine->GetSubsystem<RenderSubsystem>();
		renderSystem->GetContext()->WaitDeviceIdle();

		Runtime::Mem::Allocator::Destroy<Runtime::RHI::Pipeline>(_forwardPassPipeline);
	}

	void SceneRenderer2DSubsystem::GetDependencyGraph(DependencyGraph & graph) {
		graph.Require<AssetSubsystem>(DependencyOrder::After);
		graph.Require<RenderSubsystem>(DependencyOrder::After);
	}
}