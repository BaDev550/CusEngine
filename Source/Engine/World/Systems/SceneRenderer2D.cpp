#include "SceneRenderer2D.h"

#include <Engine/Asset/AssetSubsystem.h>
#include <Engine/Window/WindowSubsystem.h>
#include <Engine/Renderer/RenderSubsystem.h>
#include <Engine/Renderer/RenderGraph/RenderGraph.h>

#include <imgui.h>

namespace Tourqe::Engine {
	Runtime::Result SceneRenderer2DSystem::OnCreate() {
		auto assetSystem = _engine->GetSubsystem<AssetSubsystem>();
		auto* renderSystem = _engine->GetSubsystem<RenderSubsystem>();
		{
			Shader* forwardPassVertexShader = assetSystem->Get<Shader>("base_forward_vert.vert");
			Shader* forwardPassFragmentShader = assetSystem->Get<Shader>("base_forward_frag.frag");

			Runtime::RHI::PushConstantRange pcRange{};
			pcRange.size = sizeof(SpritePushConstant);
			pcRange.offset = 0;

			Runtime::RHI::PipelineDesc vertexDesc{};
			vertexDesc.colorFormats = { renderSystem->GetSwapchain()->GetColorFormat() };
			vertexDesc.depthFormat = renderSystem->GetSwapchain()->GetDepthFormat();
			vertexDesc.vertexShader = &forwardPassVertexShader->GetDesc();
			vertexDesc.fragmentShader = &forwardPassFragmentShader->GetDesc();
			vertexDesc.pushConstantRanges = { pcRange };
			vertexDesc.blending = false;
			vertexDesc.depthTest = false;

			_forwardPassPipeline = renderSystem->GetContext()->CreatePipeline(vertexDesc);
		}

		_randomTexture = assetSystem->Get<Texture2D>("dog-meme.png");

		return Runtime::Result();
	}

	void SceneRenderer2DSystem::OnUpdate(Scene& scene) {
		auto renderSystem = _engine->GetSubsystem<RenderSubsystem>();

		renderSystem->Submit([=](Runtime::RHI::CommandBuffer* cmd) {
			RenderGraph& graph = renderSystem->GetRenderGraph();

			u32 width = (u32)renderSystem->GetSwapchain()->GetExtent().x;
			u32 height = (u32)renderSystem->GetSwapchain()->GetExtent().y;

			auto backbuffer = graph.ImportImage(
				"BackBuffer",
				renderSystem->GetSwapchain()->GetColorAttachments()[renderSystem->GetImageIndex()],
				Runtime::RHI::ImageLayout::Undefined,
				Runtime::RHI::ImageLayout::PresentSrc
			);

			auto depthbuffer = graph.ImportImage(
				"DepthBuffer",
				renderSystem->GetSwapchain()->GetDepthAttachment(),
				Runtime::RHI::ImageLayout::Undefined,
				Runtime::RHI::ImageLayout::DepthAttachment
			);

			graph.AddPass("Forward",
				[&](RenderPassBuilder& builder) {
					builder.Write(backbuffer, Runtime::RHI::ImageLayout::ColorAttachment);
					builder.Write(depthbuffer, Runtime::RHI::ImageLayout::DepthAttachment);
					builder.ForceRender();
				},
				[=](Runtime::RHI::CommandBuffer* cmd, const RenderGraphResourceContainer& res) {
					auto* color = res.GetImage(backbuffer);
					auto* depth = res.GetImage(depthbuffer);

					Runtime::RHI::ColorAttachment colorAttachment{};
					colorAttachment.image = color;
					colorAttachment.loadOp = Runtime::RHI::ImageLoadOp::Load;
					colorAttachment.clearColor = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);

					Runtime::RHI::RenderingSubmitInfo info{};
					info.colorAttachments = { colorAttachment };
					info.depthAttachment = depth;
					info.extent = glm::vec2(width, height);

					Runtime::RHI::PassDebugInfo debugInfo{};
					debugInfo.label = "ForwardPass";
					debugInfo.color = glm::vec3(1.0f, 1.0f, 1.0f);

					cmd->BeginDynamicRendering(info, &debugInfo);

					SpritePushConstant pc;
					pc.textureID = _randomTexture->_image->GetBindlessIndex();
					pc.samplerID = _randomTexture->_image->GetSamplerIndex();

					_forwardPassPipeline->Bind(cmd);
					_forwardPassPipeline->PushConstant(cmd, &pc, sizeof(SpritePushConstant));
					cmd->DrawVertex(_forwardPassPipeline, 6);
					cmd->EndDynamicRendering();
				});

			graph.AddPass("Widget",
				[&](RenderPassBuilder& builder) {
					builder.Write(backbuffer, Runtime::RHI::ImageLayout::ColorAttachment);
					builder.Write(depthbuffer, Runtime::RHI::ImageLayout::DepthAttachment);
					builder.ForceRender();
				},
				[=](Runtime::RHI::CommandBuffer* cmd, const RenderGraphResourceContainer& res) {
					auto* color = res.GetImage(backbuffer);
					auto* depth = res.GetImage(depthbuffer);

					Runtime::RHI::ColorAttachment colorAttachment{};
					colorAttachment.image = color;
					colorAttachment.loadOp = Runtime::RHI::ImageLoadOp::Load;
					colorAttachment.clearColor = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);

					Runtime::RHI::RenderingSubmitInfo info{};
					info.colorAttachments = { colorAttachment };
					info.depthAttachment = depth;
					info.extent = glm::vec2(width, height);

					Runtime::RHI::PassDebugInfo debugInfo{};
					debugInfo.label = "WidgetPass";
					debugInfo.color = glm::vec3(1.0f, 0.0f, 0.0f);

					cmd->BeginDynamicRendering(info, &debugInfo);

					renderSystem->GetImGuiContext()->Flush(cmd);
					cmd->EndDynamicRendering();
				});
			});
	}

	void SceneRenderer2DSystem::OnDestroy() {
		auto renderSystem = _engine->GetSubsystem<RenderSubsystem>();

		renderSystem->GetContext()->WaitDeviceIdle();
		Runtime::Mem::Allocator::Destroy(_forwardPassPipeline);
	}
}