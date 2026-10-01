#include "VulkanCommandBuffer.h"

namespace Runtime::RHI {
	VulkanCommandBuffer::VulkanCommandBuffer(Context* context, const CommandBufferDesc& desc) : CommandBuffer(context), _desc(desc) { }

	void VulkanCommandBuffer::Begin() {
		VkCommandBufferBeginInfo begin{};
		begin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		begin.pInheritanceInfo = nullptr;
		vkBeginCommandBuffer(_commandBuffer, &begin);
	}

	void VulkanCommandBuffer::End() {
		vkEndCommandBuffer(_commandBuffer);
	}

	void VulkanCommandBuffer::Reset() {
		vkResetCommandBuffer(_commandBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
	}

	void VulkanCommandBuffer::BeginDynamicRendering(const RenderingSubmitInfo& info) {}
	void VulkanCommandBuffer::EndDynamicRendering() {}

	void VulkanCommandBuffer::TransitionImageLayout(Image* image, ImageLayout newLayout) {}
	void VulkanCommandBuffer::CopyBuffer(Buffer* srcBuffer, Buffer* dstBuffer, size_t size) {}
	void VulkanCommandBuffer::CopyBufferToImage(Buffer* buffer, Image* image, ImageLayout layout, u32 width, u32 height) {}
}