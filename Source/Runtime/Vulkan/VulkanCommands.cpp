#include <Runtime/Vulkan/VulkanCommands.h>
#include <Runtime/Vulkan/VulkanBuffer.h>
#include <Runtime/Vulkan/VulkanImage.h>

#include <Engine/Core/Engine.h>
#include <Engine/Window/WindowSubsystem.h>

#include <imgui.h>
#include <imgui_impl_vulkan.h>
#include <imgui_impl_glfw.h>

namespace CusEngine::RHI {
	VulkanCommands::VulkanCommands(const CommandsDesc& desc) : _desc(desc) {

	}

	VulkanCommands::~VulkanCommands() {
		VulkanContext* vkContext = GetContext<VulkanContext>();

		_commandQueue.clear();

		for (VkSemaphore& s : _renderFinishedSemaphores) {
			vkDestroySemaphore(vkContext->GetDevice(), s, nullptr);
		}

		for (FrameData& fd : _frames) {
			fd.TrackedObjects.clear();
			vkDestroyCommandPool(vkContext->GetDevice(), fd.CommandPool, nullptr);
			vkDestroySemaphore(vkContext->GetDevice(), fd.ImageAvailableSemaphore, nullptr);
		}

		vkDestroySemaphore(vkContext->GetDevice(), _timelineSemaphore, nullptr);
	}

	void VulkanCommands::CreateCommandPool() {
		VulkanContext* vkContext = GetContext<VulkanContext>();
		{
			for (FrameData& fd : _frames) {
				VkCommandPoolCreateInfo poolCreateInfo{};
				poolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
				poolCreateInfo.queueFamilyIndex = vkContext->GetGraphicsAndPresentQueueIndex();
				Logger::Assert((vkCreateCommandPool(vkContext->GetDevice(), &poolCreateInfo, nullptr, &fd.CommandPool) == VK_SUCCESS), GetObjectDebugName(), "Failed to create frame command pool");

				VkCommandBufferAllocateInfo allocInfo{};
				allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
				allocInfo.commandPool = fd.CommandPool;
				allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
				allocInfo.commandBufferCount = 1;
				Logger::Assert((vkAllocateCommandBuffers(vkContext->GetDevice(), &allocInfo, &fd.CommandBuffer) == VK_SUCCESS), GetObjectDebugName(), "Failed to create frame command buffer");
			}
			Logger::Info(GetObjectDebugName(), "Command pool and buffers created for frames");
		}
	}

	void VulkanCommands::CreateTimelineSemaphore() {
		VulkanContext* vkContext = GetContext<VulkanContext>();
		{
			VkSemaphoreTypeCreateInfo tlsemaphoreTypeInfo{};
			tlsemaphoreTypeInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
			tlsemaphoreTypeInfo.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
			tlsemaphoreTypeInfo.initialValue = VulkanContext::MaxFramesInFlight;

			VkSemaphoreCreateInfo tlsemaphoreCreateInfo{};
			tlsemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			tlsemaphoreCreateInfo.pNext = &tlsemaphoreTypeInfo;
			Logger::Assert((vkCreateSemaphore(vkContext->GetDevice(), &tlsemaphoreCreateInfo, nullptr, &_timelineSemaphore) == VK_SUCCESS), GetObjectDebugName(), "Failed to create timeline semaphore!");
			Logger::Info(GetObjectDebugName(), "Timeline semaphore created");
		}
	}

	void VulkanCommands::CreateRenderFinishedSemaphore() {
		VulkanContext* vkContext = GetContext<VulkanContext>();
		{
			_renderFinishedSemaphores.resize(_desc.targetSwapchain->GetImageCount());
			for (VkSemaphore& s : _renderFinishedSemaphores) {
				VkSemaphoreCreateInfo createInfo{};
				createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
				Logger::Assert((vkCreateSemaphore(vkContext->GetDevice(), &createInfo, nullptr, &s) == VK_SUCCESS), GetObjectDebugName(), "Failed to create render finished binary semaphore!");
			}

			for (FrameData& fd : _frames) {
				VkSemaphoreCreateInfo createInfo{};
				createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
				Logger::Assert((vkCreateSemaphore(vkContext->GetDevice(), &createInfo, nullptr, &fd.ImageAvailableSemaphore) == VK_SUCCESS), GetObjectDebugName(), "Failed to create image acquired binary semaphore!");
			}
			Logger::Info(GetObjectDebugName(), "Binary ImageAvailableSemaphore and RenderFinishedSemaphores are created");
		}
	}

	void VulkanCommands::BeginFrame() {
		VulkanContext* vkContext = GetContext<VulkanContext>();
		VulkanSwapchain* vkSwapchain = static_cast<VulkanSwapchain*>(_desc.targetSwapchain);

		if (_recreateSwapchainNextFrame) {
			//auto windowSubsystem = Engine::Get().GetSubsystem<WindowSubsystem>();
			//auto window = windowSubsystem->GetWindow();
			//_context->WaitDeviceIdle();
			//_swapchain->Destroy();
			//_swapchain->Recreate(800, 800);
			_recreateSwapchainNextFrame = false;
		}

		_currentFrameIndex = _signalValue++ % VulkanContext::MaxFramesInFlight;
		const uint64_t waitValue = _nextSignalValue - VulkanContext::MaxFramesInFlight;
		FrameData& frame = _frames[_currentFrameIndex];

		VkSemaphoreWaitInfo waitInfo{};
		waitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO_KHR;
		waitInfo.semaphoreCount = 1;
		waitInfo.pSemaphores = &_timelineSemaphore;
		waitInfo.pValues = &waitValue;
		vkWaitSemaphores(vkContext->GetDevice(), &waitInfo, UINT64_MAX);

		VkResult acquireResult = vkAcquireNextImageKHR(vkContext->GetDevice(), vkSwapchain->GetSwapchain(), UINT64_MAX, frame.ImageAvailableSemaphore, VK_NULL_HANDLE, &_imageIndex);
		if (acquireResult == VK_ERROR_OUT_OF_DATE_KHR) {
			_recreateSwapchainNextFrame = true;
			BeginFrame();
			return;
		}
		else if (acquireResult == VK_SUBOPTIMAL_KHR) {
			_recreateSwapchainNextFrame = true;
		}

		if (!frame.TrackedObjects.empty()) {
			frame.TrackedObjects.clear();
		}

		vkResetCommandPool(vkContext->GetDevice(), frame.CommandPool, 0);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		vkBeginCommandBuffer(frame.CommandBuffer, &beginInfo);

		_frameRecording = true;

		for (auto& command : _commandQueue) { command(); }
		_commandQueue.clear();
	}

	void VulkanCommands::EndFrame() {
		VulkanContext* vkContext = GetContext<VulkanContext>();
		VulkanSwapchain* vkSwapchain = static_cast<VulkanSwapchain*>(_desc.targetSwapchain);

		FrameData* frame = GetCurrentFrameData();

		vkEndCommandBuffer(frame->CommandBuffer);

		VkSemaphoreSubmitInfo imageAcquireWaitInfo{};
		imageAcquireWaitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
		imageAcquireWaitInfo.semaphore = frame->ImageAvailableSemaphore;
		imageAcquireWaitInfo.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;

		std::vector<VkSemaphoreSubmitInfo> semaphoreSignals(2);
		semaphoreSignals[0].sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
		semaphoreSignals[0].semaphore = _renderFinishedSemaphores[_imageIndex];
		semaphoreSignals[0].stageMask = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT;

		semaphoreSignals[1].sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
		semaphoreSignals[1].semaphore = _timelineSemaphore;
		semaphoreSignals[1].value = _nextSignalValue++;
		semaphoreSignals[1].stageMask = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT;

		VkCommandBufferSubmitInfo cmdSubmitInfo{};
		cmdSubmitInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
		cmdSubmitInfo.commandBuffer = frame->CommandBuffer;

		VkSubmitInfo2 submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
		submitInfo.waitSemaphoreInfoCount = 1;
		submitInfo.pWaitSemaphoreInfos = &imageAcquireWaitInfo;
		submitInfo.commandBufferInfoCount = 1;
		submitInfo.pCommandBufferInfos = &cmdSubmitInfo;
		submitInfo.signalSemaphoreInfoCount = static_cast<uint32_t>(semaphoreSignals.size());
		submitInfo.pSignalSemaphoreInfos = semaphoreSignals.data();

		vkQueueSubmit2(vkContext->GetGraphicsAndPresentQueue(), 1, &submitInfo, VK_NULL_HANDLE);

		_nextSignalValue++;
		_signalValue++;

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &_renderFinishedSemaphores[_imageIndex];
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &vkSwapchain->GetSwapchain();
		presentInfo.pImageIndices = &_imageIndex;
		presentInfo.pResults = nullptr;
		vkQueuePresentKHR(vkContext->GetGraphicsAndPresentQueue(), &presentInfo);

		_frameRecording = false;
	}

	void VulkanCommands::BeginImGui() {
		_context->NewFrameImGui();
	}

	void VulkanCommands::EndImGui() {
		FrameData* fd = GetCurrentFrameData();

		ImGui::Render();
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), fd->CommandBuffer);
	}

	void VulkanCommands::Submit(CommandFunc func) { _commandQueue.push_back(std::move(func)); }
	void VulkanCommands::Track(RHIObject* object) { GetCurrentFrameData()->TrackedObjects.push_back(object); }

	void VulkanCommands::Wait() {
		_context->WaitDeviceIdle();
	}

	void VulkanCommands::BeginDynamicRendering(std::vector<Image*> colorAttachments, Image* depthAttachment, glm::vec2 extent, glm::vec4 clearColor) {
		VulkanContext* vkContext = GetContext<VulkanContext>();

		FrameData* fd = GetCurrentFrameData();
		uint32_t colorAttachmentCount = static_cast<uint32_t>(colorAttachments.size());

		std::vector<VkRenderingAttachmentInfo> colorAttachmentInfos(colorAttachmentCount);
		for (uint32_t i = 0; i < colorAttachmentCount; i++) {
			auto& attachment = colorAttachmentInfos[i];
			VulkanImage* vkImage = static_cast<VulkanImage*>(colorAttachments[i]);
			attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
			attachment.imageView = vkImage->GetImageView();
			attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			attachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			attachment.clearValue.color = { {clearColor.x, clearColor.y, clearColor.z, clearColor.w} };
			vkContext->TransitionImageLayout(fd->CommandBuffer, vkImage, ImageLayout::ColorAttachment);
		}

		VkRenderingAttachmentInfo depthAttachmentInfo{};
		VulkanImage* vkDepthImage = static_cast<VulkanImage*>(depthAttachment);
		depthAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		depthAttachmentInfo.imageView = vkDepthImage->GetImageView();
		depthAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		depthAttachmentInfo.clearValue.depthStencil = { 1.0f, 0 };
		vkContext->TransitionImageLayout(fd->CommandBuffer, vkDepthImage, ImageLayout::DepthAttachment);

		VkRect2D renderArea{};
		renderArea.offset.x = 0;
		renderArea.offset.y = 0;
		renderArea.extent = VkExtent2D(extent.x, extent.y);

		VkRenderingInfo info{};
		info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
		info.renderArea = renderArea;
		info.layerCount = 1;
		info.colorAttachmentCount = colorAttachmentCount;
		info.pColorAttachments = colorAttachmentInfos.data();
		info.pDepthAttachment = &depthAttachmentInfo;

		vkCmdBeginRendering(fd->CommandBuffer, &info);

		{
			VkViewport viewport{};
			viewport.x = 0;
			viewport.y = 0;
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			viewport.width = extent.x;
			viewport.height = extent.y;
			vkCmdSetViewport(fd->CommandBuffer, 0, 1, &viewport);

			VkRect2D scissor{};
			scissor.offset.x = 0;
			scissor.offset.y = 0;
			scissor.extent = VkExtent2D(extent.x, extent.y);
			vkCmdSetScissor(fd->CommandBuffer, 0, 1, &scissor);
		}
	}

	void VulkanCommands::EndDynamicRendering() {
		FrameData* fd = GetCurrentFrameData();
		vkCmdEndRendering(fd->CommandBuffer);
	}

	void VulkanCommands::TransitionImageLayout(Image* image, ImageLayout newLayout) {
		VulkanContext* vkContext = GetContext<VulkanContext>();

		FrameData* fd = GetCurrentFrameData();
		vkContext->TransitionImageLayout(fd->CommandBuffer, image, newLayout);
	}

	void VulkanCommands::CopyBuffer(Buffer* srcBuffer, Buffer* dstBuffer, size_t size) {
		VulkanContext* vkContext = GetContext<VulkanContext>();

		FrameData* fd = GetCurrentFrameData();
		VulkanBuffer* vkSrcBuffer = static_cast<VulkanBuffer*>(srcBuffer);
		VulkanBuffer* vkDstBuffer = static_cast<VulkanBuffer*>(dstBuffer);
		vkContext->CopyBuffer(fd->CommandBuffer, size, vkSrcBuffer->GetVkBuffer(), vkDstBuffer->GetVkBuffer());
	}

	void VulkanCommands::CopyBufferToImage(Buffer* buffer, Image* image, ImageLayout layout, u32 width, u32 height) {
		VulkanContext* vkContext = GetContext<VulkanContext>();

		FrameData* fd = GetCurrentFrameData();
		VulkanBuffer* vkBuffer = static_cast<VulkanBuffer*>(buffer);
		VulkanImage* vkImage = static_cast<VulkanImage*>(image);
		VkImageLayout vkLayout = Utils::GetVkImageLayout(layout);
		vkContext->CopyBufferToImage(fd->CommandBuffer, vkBuffer->GetVkBuffer(), vkImage->GetImage(), vkLayout, width, height);
	}

	uint32_t VulkanCommands::GetImageIndex() const noexcept { return _imageIndex; }

	VulkanCommands::FrameData* VulkanCommands::GetCurrentFrameData()
	{
		Logger::Assert(_frameRecording, GetObjectDebugName(), "No active recording!");
		return &_frames[_currentFrameIndex];
	}
}