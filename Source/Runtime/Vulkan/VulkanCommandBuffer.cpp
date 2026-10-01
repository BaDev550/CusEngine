#include "VulkanCommandBuffer.h"
#include "VulkanContext.h"
#include "VulkanImage.h"
#include "VulkanBuffer.h"
#include "VulkanUtils.h"

#include <imgui.h>
#include <imgui_impl_vulkan.h>
#include <imgui_impl_glfw.h>

namespace Runtime::RHI {
	VulkanCommandBuffer::VulkanCommandBuffer(Context* context, const CommandBufferDesc& desc) : CommandBuffer(context), _desc(desc) { }

	void VulkanCommandBuffer::Begin() {
		VkCommandBufferBeginInfo begin{};
		begin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		vkBeginCommandBuffer(_commandBuffer, &begin);
	}

	void VulkanCommandBuffer::End() {
		vkEndCommandBuffer(_commandBuffer);
	}

	void VulkanCommandBuffer::BeginImGui() {
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
	}

	void VulkanCommandBuffer::RenderImGui() {
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), _commandBuffer);
	}

	void VulkanCommandBuffer::Reset() {
		vkResetCommandBuffer(_commandBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
	}

	void VulkanCommandBuffer::BeginDynamicRendering(const RenderingSubmitInfo& info) {
		VulkanContext* vkContext = GetOwningRHIContext<VulkanContext>();

		u32 colorAttachmentCount = static_cast<uint32_t>(info.colorAttachments.size());

		std::vector<VkRenderingAttachmentInfo> colorAttachmentInfos(colorAttachmentCount);
		for (uint32_t i = 0; i < colorAttachmentCount; i++) {
			auto& attachmentInfo = colorAttachmentInfos[i];
			auto& colorAttachment = info.colorAttachments[i];

			VulkanImage* vkImage = static_cast<VulkanImage*>(colorAttachment.image);
			attachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
			attachmentInfo.imageView = vkImage->GetImageView();
			attachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			attachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			attachmentInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			attachmentInfo.clearValue.color = { 
				{
					colorAttachment.clearColor.x, 
					colorAttachment.clearColor.y, 
					colorAttachment.clearColor.z, 
					colorAttachment.clearColor.w
				} };
			vkContext->TransitionImageLayout(_commandBuffer, vkImage, ImageLayout::ColorAttachment);
		}

		VkRenderingAttachmentInfo depthAttachmentInfo{};
		VulkanImage* vkDepthImage = static_cast<VulkanImage*>(info.depthAttachment);
		depthAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		depthAttachmentInfo.imageView = vkDepthImage->GetImageView();
		depthAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		depthAttachmentInfo.clearValue.depthStencil = { 1.0f, 0 };
		vkContext->TransitionImageLayout(_commandBuffer, vkDepthImage, ImageLayout::DepthAttachment);

		VkRect2D renderArea{};
		renderArea.offset.x = 0;
		renderArea.offset.y = 0;
		renderArea.extent = VkExtent2D(info.extent.x, info.extent.y);

		VkRenderingInfo renderInfo{};
		renderInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
		renderInfo.renderArea = renderArea;
		renderInfo.layerCount = 1;
		renderInfo.colorAttachmentCount = colorAttachmentCount;
		renderInfo.pColorAttachments = colorAttachmentInfos.data();
		renderInfo.pDepthAttachment = &depthAttachmentInfo;

		vkCmdBeginRendering(_commandBuffer, &renderInfo);

		{
			VkViewport viewport{};
			viewport.x = 0;
			viewport.y = 0;
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			viewport.width = info.extent.x;
			viewport.height = info.extent.y;
			vkCmdSetViewport(_commandBuffer, 0, 1, &viewport);

			VkRect2D scissor{};
			scissor.offset.x = 0;
			scissor.offset.y = 0;
			scissor.extent = VkExtent2D(info.extent.x, info.extent.y);
			vkCmdSetScissor(_commandBuffer, 0, 1, &scissor);
		}
	}

	void VulkanCommandBuffer::EndDynamicRendering() {
		vkCmdEndRendering(_commandBuffer);
	}

	void VulkanCommandBuffer::TransitionImageLayout(Image* image, ImageLayout newLayout) {
		VulkanContext* vkContext = GetOwningRHIContext<VulkanContext>();
		vkContext->TransitionImageLayout(_commandBuffer, image, newLayout);
	}

	void VulkanCommandBuffer::CopyBuffer(Buffer* srcBuffer, Buffer* dstBuffer, size_t size) {
		VulkanContext* vkContext = GetOwningRHIContext<VulkanContext>();
		VulkanBuffer* vkSrcBuffer = static_cast<VulkanBuffer*>(srcBuffer);
		VulkanBuffer* vkDstBuffer = static_cast<VulkanBuffer*>(dstBuffer);
		vkContext->CopyBuffer(_commandBuffer, size, vkSrcBuffer->GetVkBuffer(), vkDstBuffer->GetVkBuffer());
	}

	void VulkanCommandBuffer::CopyBufferToImage(Buffer* buffer, Image* image, ImageLayout layout, u32 width, u32 height) {
		VulkanContext* vkContext = GetOwningRHIContext<VulkanContext>();
		VulkanBuffer* vkBuffer = static_cast<VulkanBuffer*>(buffer);
		VulkanImage* vkImage = static_cast<VulkanImage*>(image);
		VkImageLayout vkLayout = Utils::GetVkImageLayout(layout);
		vkContext->CopyBufferToImage(_commandBuffer, vkBuffer->GetVkBuffer(), vkImage->GetImage(), vkLayout, width, height);
	}
}