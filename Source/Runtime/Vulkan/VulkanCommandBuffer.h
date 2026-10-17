#pragma once

#include <Runtime/RHI/Command/RHICommandBuffer.h>
#include <Runtime/RHI/Command/RHICommandBufferDesc.h>
#include <vulkan/vulkan.h>

namespace Runtime::RHI {
	class VulkanCommandBuffer : public CommandBuffer {
	public:
		VulkanCommandBuffer(Context* context, const CommandBufferDesc& desc);
		virtual ~VulkanCommandBuffer() = default;

		virtual void Begin() override;
		virtual void End() override;
		virtual void Reset() override;

		virtual void BeginDynamicRendering(const RenderingSubmitInfo& info, PassDebugInfo* debugInfo = nullptr) override;
		virtual void EndDynamicRendering() override;

		virtual void TransitionImageLayout(Image* image, ImageLayout newLayout) override;
		virtual void CopyBuffer(Buffer* srcBuffer, Buffer* dstBuffer, size_t size) override;
		virtual void CopyBufferToImage(Buffer* buffer, Image* image, ImageLayout layout, u32 width, u32 height) override;

		virtual void DrawVertex(Pipeline* pipeline, u32 count) override;

		[[nodiscard]] VkCommandBuffer GetVkCommandBuffer() const { return _commandBuffer; }
	private:
		CommandBufferDesc _desc;
		VkCommandBuffer _commandBuffer = VK_NULL_HANDLE;
		b8 _debugPassAttached = false;

		friend class VulkanCommandPool;
	};
}