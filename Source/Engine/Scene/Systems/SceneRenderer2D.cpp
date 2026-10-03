#include "SceneRenderer2D.h"

#include <Engine/Asset/AssetSubsystem.h>
#include <Engine/Window/WindowSubsystem.h>
#include <Engine/Renderer/RenderSubsystem.h>

#include <imgui.h>

namespace CusEngine {
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

		_testSprite = assetSystem->Get<Texture2D>("guven-catak.jpg");

		return Runtime::Result();
	}

	void SceneRenderer2DSystem::OnUpdate(Scene& scene) {
		auto renderSystem = _engine->GetSubsystem<RenderSubsystem>();

		renderSystem->Pass([=](Runtime::RHI::CommandBuffer* cmd) {
			auto context = renderSystem->GetContext();
			auto swapchain = renderSystem->GetSwapchain();

			if (_testSprite->GetAssetState() == AssetState::Ready) {
				SpritePushConstant pc;
				pc.textureID = _testSprite->_image->GetBindlessIndex();
				pc.samplerID = _testSprite->_image->GetSamplerIndex();

				_forwardPassPipeline->Bind(cmd);
				_forwardPassPipeline->PushConstant(cmd, &pc, sizeof(SpritePushConstant), 0);
				cmd->DrawVertex(_forwardPassPipeline, 6);
			}
			});
	}

	void SceneRenderer2DSystem::OnDestroy() {
		auto renderSystem = _engine->GetSubsystem<RenderSubsystem>();
		renderSystem->GetContext()->WaitDeviceIdle();

		Runtime::Mem::Allocator::Destroy<Runtime::RHI::Pipeline>(_forwardPassPipeline);
	}
}