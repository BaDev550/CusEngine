#pragma once
#include <Renderer/RenderSubsystem.h>
#include <Graphics/RHI/RHISubsystem.h>

namespace CusEngine {
	using namespace RHI;

#define CHECK_DEPENDEND(depended) Logger::Assert(depended, "RHI", "Invalid context")
#define DESTROY_OBJECT_CHECKED(obj) if (obj) delete obj

	CUS_CLASS()
	class ENGINE_API Vulkan_RHISubsystem final : public RHISubsystem {
		REFLECT_CLASS(Vulkan_RHISubsystem)
	public:
		virtual [[nodiscard]] RenderContext* CreateRenderContext(const RenderContextDesc& desc) override;
		virtual void DestroyRenderContext(RenderContext* context) override;

		virtual [[nodiscard]] RenderCommands* CreateRenderCommands(RenderContext* context, Swapchain* swapchain) override;
		virtual void DestroyRenderCommands(RenderCommands* commands) override;

		virtual [[nodiscard]] Swapchain* CreateSwapchain(RenderContext* context, const SwapchainDesc& desc) override;
		virtual void DestroySwapchain(Swapchain* swapchain) override;

		virtual [[nodiscard]] Image* CreateImage(RenderCommands* commands, const ImageDesc& desc) override;
		virtual [[nodiscard]] Buffer* CreateBuffer(RenderCommands* commands, const BufferDesc& desc) override;
		virtual [[nodiscard]] Pipeline* CreatePipeline(RenderCommands* commands, const PipelineDesc& desc) override;
	};
}