#pragma once

#include "Graphics/RHI/RHI_RenderCommands.h"

#include "OpenGLRenderContext.h"
#include "OpenGLSwapchain.h"

namespace CusEngine::RHI {
	class OpenGL_RenderCommands : public RenderCommands {
	public:
		OpenGL_RenderCommands(OpenGL_RenderContext* context, OpenGL_Swapchain* swapchain);
		virtual ~OpenGL_RenderCommands();

		virtual void BeginFrame() override final;
		virtual void EndFrame() override final;
		virtual void Submit(CommandFunc func) override final;
		virtual void Track(const Mem::Ref<RenderObject>& object) override final;
		virtual void Wait() override final;

		virtual void BeginDynamicRendering(std::vector<Mem::Ref<Image>> colorAttachments, Mem::Ref<Image> depthAttachment, glm::vec2 extent, glm::vec4 clearColor = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f)) override final;
		virtual void EndDynamicRendering() override final;

		virtual void TransitionImageLayout(Image* image, ImageLayout newLayout) override;
		virtual void CopyBuffer(Buffer* srcBuffer, Buffer* dstBuffer, size_t size) override;
		virtual void CopyBufferToImage(Buffer* buffer, Image* image, ImageLayout layout, u32 width, u32 height) override;
		virtual u32 RegisterBindlessImage(const Mem::Ref<Image>& image) override;

		virtual [[nodiscard]] uint32_t GetImageIndex() const noexcept override final;
		virtual [[nodiscard]] Swapchain* GetTargetSwapchain() const override final;
		virtual [[nodiscard]] RenderContext* GetTargetRenderContext() const override final;

		[[nodiscard]] OpenGL_RenderContext* GetGLContext() const { return _context; }
	private:
		OpenGL_RenderContext* _context = nullptr;
		OpenGL_Swapchain* _swapchain = nullptr;

		std::vector<Mem::Ref<Image>> _bindlessImages;

		bool _recreateSwapchainNextFrame = false;
	};
}