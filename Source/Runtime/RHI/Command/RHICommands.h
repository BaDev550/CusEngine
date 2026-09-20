#pragma once
#include <Engine/Core/Core.h>
#include <glm/glm.hpp>
#include <functional>

#include <Runtime/RHI/Object/RHIObject.h>
#include <Runtime/RHI/Image/RHIImageLayout.h>
#include <Runtime/RHI/Command/RHICommandsDesc.h>

namespace CusEngine::RHI {
	class Image;
	class Buffer;
	
	using CommandFunc = std::function<void()>;

	class ENGINE_API Commands : public RHIObject {
	public:
		Commands() = default;
		virtual ~Commands() = default;

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;
		virtual void Submit(CommandFunc func) = 0;
		virtual void Track(RHIObject* object) = 0;
		virtual void Wait() = 0;

		virtual void BeginDynamicRendering(std::vector<Image*> colorAttachments, Image* depthAttachment, glm::vec2 extent, glm::vec4 clearColor = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f)) = 0;
		virtual void EndDynamicRendering() = 0;
		
		virtual void TransitionImageLayout(Image* image, ImageLayout newLayout) = 0;
		virtual void CopyBuffer(Buffer* srcBuffer, Buffer* dstBuffer, size_t size) = 0;
		virtual void CopyBufferToImage(Buffer* buffer, Image* image, ImageLayout layout, u32 width, u32 height) = 0;
		virtual const CommandsDesc& GetDesc() const = 0;

		virtual [[nodiscard]] uint32_t GetImageIndex() const noexcept = 0;
	};
}