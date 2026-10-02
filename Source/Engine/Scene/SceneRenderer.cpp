#include "SceneRenderer.h"

#include <Engine/Asset/AssetSubsystem.h>
#include <Engine/Window/WindowSubsystem.h>
#include <Engine/Asset/Texture/Texture2D.h>

#include <imgui.h>

namespace CusEngine {
	Runtime::Result SceneRendererSubsystem::OnCreate(Engine* engine) {
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

	void SceneRendererSubsystem::OnUpdate() {
		auto renderSystem = _engine->GetSubsystem<RenderSubsystem>();
		auto context = renderSystem->GetContext();
		auto swapchain = renderSystem->GetSwapchain();

		renderSystem->BeginFrame();

		Runtime::RHI::CommandBuffer* cmd = renderSystem->GetCurrentFrameData()->commandBuffer;

		cmd->BeginImGui();
		ImGui::NewFrame();

		ImGui::ShowDemoWindow();

		renderSystem->BeginSwapchainPass();

		if (_testSprite->GetAssetState() == AssetState::Ready) {
			SpritePushConstant pc;
			pc.textureID = _testSprite->_image->GetBindlessIndex();
			pc.samplerID = _testSprite->_image->GetSamplerIndex();

			_forwardPassPipeline->Bind(cmd);
			_forwardPassPipeline->PushConstant(cmd, &pc, sizeof(SpritePushConstant), 0);
			cmd->DrawVertex(_forwardPassPipeline, 6);
		}

		ImGui::Render();
		cmd->RenderImGui();

		renderSystem->EndSwapchainPass();
		renderSystem->EndFrame();
	}

	void SceneRendererSubsystem::OnDestroy() {
		Subsystem::OnDestroy();

		auto renderSystem = _engine->GetSubsystem<RenderSubsystem>();
		renderSystem->GetContext()->WaitDeviceIdle();

		Runtime::Mem::Allocator::Destroy<Runtime::RHI::Pipeline>(_forwardPassPipeline);
	}

	void SceneRendererSubsystem::GetDependencyGraph(DependencyGraph & graph) {
		graph.Require<AssetSubsystem>(DependencyOrder::After);
		graph.Require<RenderSubsystem>(DependencyOrder::After);
	}
}