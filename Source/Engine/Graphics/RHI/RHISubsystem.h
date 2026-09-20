#pragma once
#include "Subsystem/Subsystem.h"
#include "Core/Core.h"
#include "RHI.h"

namespace CusEngine::RHI {
	CUS_CLASS()
	class ENGINE_API RHISubsystem : public Subsystem {
		REFLECT_CLASS(RHISubsystem)
	public:
		virtual ~RHISubsystem() = default;

		virtual bool OnCreate(Engine* engine) override { Subsystem::OnCreate(engine); Logger::Info("RHI", "Created!"); return true; }
		virtual void OnUpdate() override {}
		virtual void OnDestroy() override {}
		virtual void GetDependencyGraph(DependencyGraph& graph) override;

		virtual [[nodiscard]] RenderContext* CreateRenderContext(const RenderContextDesc& desc) { return nullptr; };
		virtual void DestroyRenderContext(RenderContext* context) {};
		virtual [[nodiscard]] RenderCommands* CreateRenderCommands(RenderContext* context, Swapchain* swapchain) { return nullptr; };
		virtual void DestroyRenderCommands(RenderCommands* commands) {};
		virtual [[nodiscard]] Swapchain* CreateSwapchain(RenderContext* context, const SwapchainDesc& desc) { return nullptr; };
		virtual void DestroySwapchain(Swapchain* swapchain) {};
		virtual [[nodiscard]] Image* CreateImage(RenderCommands* commands, const ImageDesc& desc) { return nullptr; };
		virtual [[nodiscard]] Buffer* CreateBuffer(RenderCommands* commands, const BufferDesc& desc) { return nullptr; };
		virtual [[nodiscard]] Pipeline* CreatePipeline(RenderCommands* commands, const PipelineDesc& desc) { return nullptr; };

		//static inline RHI::GraphicsBackend Backend = RHI::GraphicsBackend::Vulkan;
	};
}