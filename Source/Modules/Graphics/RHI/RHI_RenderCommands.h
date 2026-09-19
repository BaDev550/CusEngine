#pragma once

#include "RHI.h"

namespace Graphics {
	class ENGINE_API RHI_RenderCommands {
	public:
		RHI_RenderCommands() = default;
		virtual ~RHI_RenderCommands() = default;

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;
		virtual void Submit(RHI_CommandFunc func) = 0;
		virtual void Track(const Memory::Ref<RHI_Object>& object) = 0;
		virtual void Wait() = 0;

		virtual void BeginDynamicRendering(std::vector<Memory::Ref<RHI_Image>> colorAttachments, Memory::Ref<RHI_Image> depthAttachment, glm::vec2 extent, glm::vec4 clearColor = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f)) = 0;
		virtual void EndDynamicRendering() = 0;
		
		virtual void TransitionImageLayout(RHI_Image* image, RHI_ImageLayout newLayout) = 0;
		virtual void CopyBuffer(RHI_Buffer* srcBuffer, RHI_Buffer* dstBuffer, size_t size) = 0;
		virtual void CopyBufferToImage(RHI_Buffer* buffer, RHI_Image* image, RHI_ImageLayout layout, u32 width, u32 height) = 0;
		virtual u32 RegisterBindlessImage(const Memory::Ref<RHI_Image>& image) = 0;

		virtual [[nodiscard]] uint32_t GetImageIndex() const noexcept = 0;
		virtual [[nodiscard]] RHI_Swapchain* GetTargetSwapchain() const = 0;
		virtual [[nodiscard]] RHI_RenderContext* GetTargetRenderContext() const = 0;
	};
}