#pragma once

#include "RHI.h"
#include "Core/Ref.h"

namespace CusEngine::RHI {
	class ENGINE_API RenderCommands {
	public:
		RenderCommands() = default;
		virtual ~RenderCommands() = default;

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;
		virtual void Submit(CommandFunc func) = 0;
		virtual void Track(const Mem::Ref<RenderObject>& object) = 0;
		virtual void Wait() = 0;

		virtual void BeginDynamicRendering(std::vector<Mem::Ref<Image>> colorAttachments, Mem::Ref<Image> depthAttachment, glm::vec2 extent, glm::vec4 clearColor = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f)) = 0;
		virtual void EndDynamicRendering() = 0;
		
		virtual void TransitionImageLayout(Image* image, ImageLayout newLayout) = 0;
		virtual void CopyBuffer(Buffer* srcBuffer, Buffer* dstBuffer, size_t size) = 0;
		virtual void CopyBufferToImage(Buffer* buffer, Image* image, ImageLayout layout, u32 width, u32 height) = 0;
		virtual u32 RegisterBindlessImage(const Mem::Ref<Image>& image) = 0;

		virtual [[nodiscard]] uint32_t GetImageIndex() const noexcept = 0;
		virtual [[nodiscard]] Swapchain* GetTargetSwapchain() const = 0;
		virtual [[nodiscard]] RenderContext* GetTargetRenderContext() const = 0;
	};
}