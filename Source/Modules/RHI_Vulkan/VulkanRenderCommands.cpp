#include "VulkanRenderCommands.h"

#include "VulkanBuffer.h"
#include "VulkanImage.h"

#include <Core/Engine.h>
#include <Graphics/Framebuffer.h>
#include <Window/WindowSubsystem.h>

namespace CusEngine::RHI {
	Vulkan_RenderCommands::Vulkan_RenderCommands(Vulkan_RenderContext* context, Vulkan_Swapchain* swapchain) : _context(context), _swapchain(swapchain) {
		{
			for (FrameData& fd : _frames) {
				VkCommandPoolCreateInfo poolCreateInfo{};
				poolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
				poolCreateInfo.queueFamilyIndex = _context->GetGraphicsAndPresentQueueIndex();
				Logger::Assert((vkCreateCommandPool(_context->GetVkDeviceHandle(), &poolCreateInfo, nullptr, &fd.CommandPool) == VK_SUCCESS), "Vulkan Render Commands", "Failed to create frame command pool");

				VkCommandBufferAllocateInfo allocInfo{};
				allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
				allocInfo.commandPool = fd.CommandPool;
				allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
				allocInfo.commandBufferCount = 1;
				Logger::Assert((vkAllocateCommandBuffers(_context->GetVkDeviceHandle(), &allocInfo, &fd.CommandBuffer) == VK_SUCCESS), "Vulkan Render Commands", "Failed to create frame command buffer");
			}
			Logger::Info("Vulkan Render Commands", "Command pool and buffers created for frames");
		}
		{
			VkSemaphoreTypeCreateInfo tlsemaphoreTypeInfo{};
			tlsemaphoreTypeInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
			tlsemaphoreTypeInfo.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
			tlsemaphoreTypeInfo.initialValue = Vulkan_RenderContext::MaxFramesInFlight;

			VkSemaphoreCreateInfo tlsemaphoreCreateInfo{};
			tlsemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			tlsemaphoreCreateInfo.pNext = &tlsemaphoreTypeInfo;
			Logger::Assert((vkCreateSemaphore(_context->GetVkDeviceHandle(), &tlsemaphoreCreateInfo, nullptr, &_timelineSemaphore) == VK_SUCCESS), "Vulkan Render Commands", "Failed to create timeline semaphore!");
			Logger::Info("Vulkan Render Commands", "Timeline semaphore created");
		}
		{
			_renderFinishedSemaphores.resize(_swapchain->GetImageCount());
			for (VkSemaphore& s : _renderFinishedSemaphores) {
				VkSemaphoreCreateInfo createInfo{};
				createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
				Logger::Assert((vkCreateSemaphore(_context->GetVkDeviceHandle(), &createInfo, nullptr, &s) == VK_SUCCESS), "Vulkan Render Commands", "Failed to create render finished binary semaphore!");
			}

			for (FrameData& fd : _frames) {
				VkSemaphoreCreateInfo createInfo{};
				createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
				Logger::Assert((vkCreateSemaphore(_context->GetVkDeviceHandle(), &createInfo, nullptr, &fd.ImageAvailableSemaphore) == VK_SUCCESS), "Vulkan Render Commands", "Failed to create image acquired binary semaphore!");
			}
			Logger::Info("Vulkan Render Commands", "Binary ImageAvailableSemaphore and RenderFinishedSemaphores are created");
		}
	}

	Vulkan_RenderCommands::~Vulkan_RenderCommands() {
		_bindlessImages.clear();

		_commandQueue.clear();

		for (VkSemaphore& s : _renderFinishedSemaphores) {
			vkDestroySemaphore(_context->GetVkDeviceHandle(), s, nullptr);
		}

		for (FrameData& fd : _frames) {
			fd.TrackedObjects.clear();
			vkDestroyCommandPool(_context->GetVkDeviceHandle(), fd.CommandPool, nullptr);
			vkDestroySemaphore(_context->GetVkDeviceHandle(), fd.ImageAvailableSemaphore, nullptr);
		}

		vkDestroySemaphore(_context->GetVkDeviceHandle(), _timelineSemaphore, nullptr);
	}

	void Vulkan_RenderCommands::BeginFrame() {
		if (_recreateSwapchainNextFrame) {
			//auto windowSubsystem = Engine::Get().GetSubsystem<WindowSubsystem>();
			//auto window = windowSubsystem->GetWindow();
			//
			//_context->WaitDeviceIdle();
			//_swapchain->Destroy();
			//_swapchain->Recreate(window->GetWidth(), window->GetHeight());
			_recreateSwapchainNextFrame = false;
		}

		_currentFrameIndex = _signalValue++ % Vulkan_RenderContext::MaxFramesInFlight;
		const uint64_t waitValue = _nextSignalValue - Vulkan_RenderContext::MaxFramesInFlight;
		FrameData& frame = _frames[_currentFrameIndex];

		VkSemaphoreWaitInfo waitInfo{};
		waitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO_KHR;
		waitInfo.semaphoreCount = 1;
		waitInfo.pSemaphores = &_timelineSemaphore;
		waitInfo.pValues = &waitValue;
		vkWaitSemaphores(_context->GetVkDeviceHandle(), &waitInfo, UINT64_MAX);

		VkResult acquireResult = vkAcquireNextImageKHR(_context->GetVkDeviceHandle(), _swapchain->GetVkSwapchainHandle(), UINT64_MAX, frame.ImageAvailableSemaphore, VK_NULL_HANDLE, &_imageIndex);
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

		vkResetCommandPool(_context->GetVkDeviceHandle(), frame.CommandPool, 0);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		vkBeginCommandBuffer(frame.CommandBuffer, &beginInfo);

		_frameRecording = true;

		for (auto& command : _commandQueue) { command(); }
		_commandQueue.clear();
	}

	void Vulkan_RenderCommands::EndFrame() {
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

		vkQueueSubmit2(_context->GetGraphicsAndPresentQueue(), 1, &submitInfo, VK_NULL_HANDLE);

		_nextSignalValue++;
		_signalValue++;

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &_renderFinishedSemaphores[_imageIndex];
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &_swapchain->GetVkSwapchainHandle();
		presentInfo.pImageIndices = &_imageIndex;
		presentInfo.pResults = nullptr;
		vkQueuePresentKHR(_context->GetGraphicsAndPresentQueue(), &presentInfo);

		_frameRecording = false;
	}

	void Vulkan_RenderCommands::Submit(CommandFunc func) { _commandQueue.push_back(std::move(func)); }
	void Vulkan_RenderCommands::Track(const Mem::Ref<RenderObject>& object) { GetCurrentFrameData()->TrackedObjects.push_back(object); }

	void Vulkan_RenderCommands::Wait() {
		_context->WaitDeviceIdle();
	}

	void Vulkan_RenderCommands::BeginDynamicRendering(std::vector<Mem::Ref<Image>> colorAttachments, Mem::Ref<Image> depthAttachment, glm::vec2 extent, glm::vec4 clearColor) {
		FrameData* fd = GetCurrentFrameData();
		uint32_t colorAttachmentCount = static_cast<uint32_t>(colorAttachments.size());

		std::vector<VkRenderingAttachmentInfo> colorAttachmentInfos(colorAttachmentCount);
		for (uint32_t i = 0; i < colorAttachmentCount; i++) {
			auto& attachment = colorAttachmentInfos[i];
			Vulkan_Image* vkImage = static_cast<Vulkan_Image*>(colorAttachments[i].Get());
			attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
			attachment.imageView = vkImage->GetVkImageView();
			attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			attachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			attachment.clearValue.color = { {clearColor.x, clearColor.y, clearColor.z, clearColor.w} };
			_context->TransitionImageLayout(fd->CommandBuffer, vkImage, ImageLayout::ColorAttachment);
		}

		VkRenderingAttachmentInfo depthAttachmentInfo{};
		Vulkan_Image* vkDepthImage = static_cast<Vulkan_Image*>(depthAttachment.Get());
		depthAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		depthAttachmentInfo.imageView = vkDepthImage->GetVkImageView();
		depthAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		depthAttachmentInfo.clearValue.depthStencil = { 1.0f, 0 };
		_context->TransitionImageLayout(fd->CommandBuffer, vkDepthImage, ImageLayout::DepthAttachment);

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

	void Vulkan_RenderCommands::EndDynamicRendering() {
		FrameData* fd = GetCurrentFrameData();
		vkCmdEndRendering(fd->CommandBuffer);
	}

	void Vulkan_RenderCommands::TransitionImageLayout(Image* image, ImageLayout newLayout) {
		FrameData* fd = GetCurrentFrameData();
		_context->TransitionImageLayout(fd->CommandBuffer, image, newLayout);
	}

	void Vulkan_RenderCommands::CopyBuffer(Buffer* srcBuffer, Buffer* dstBuffer, size_t size) {
		FrameData* fd = GetCurrentFrameData();
		Vulkan_Buffer* vkSrcBuffer = static_cast<Vulkan_Buffer*>(srcBuffer);
		Vulkan_Buffer* vkDstBuffer = static_cast<Vulkan_Buffer*>(dstBuffer);
		_context->CopyBuffer(fd->CommandBuffer, size, vkSrcBuffer->GetVkBuffer(), vkDstBuffer->GetVkBuffer());
	}

	void Vulkan_RenderCommands::CopyBufferToImage(Buffer* buffer, Image* image, ImageLayout layout, u32 width, u32 height) {
		FrameData* fd = GetCurrentFrameData();
		Vulkan_Buffer* vkBuffer = static_cast<Vulkan_Buffer*>(buffer);
		Vulkan_Image* vkImage = static_cast<Vulkan_Image*>(image);
		VkImageLayout vkLayout = Utils::GetVkImageLayout(layout);
		_context->CopyBufferToImage(fd->CommandBuffer, vkBuffer->GetVkBuffer(), vkImage->GetVkImage(), vkLayout, width, height);
	}

	u32 Vulkan_RenderCommands::RegisterBindlessImage(const Mem::Ref<Image>& image) {
		u32 index = _bindlessImages.size();
		_bindlessImages.push_back(image);
		return index;
	}

	uint32_t Vulkan_RenderCommands::GetImageIndex() const noexcept { return _imageIndex; }

	Swapchain* Vulkan_RenderCommands::GetTargetSwapchain() const { return _swapchain; }
	RenderContext* Vulkan_RenderCommands::GetTargetRenderContext() const { return _context; }

	Vulkan_RenderCommands::FrameData* Vulkan_RenderCommands::GetCurrentFrameData()
	{
		Logger::Assert(_frameRecording, "Vulkan Render Commands", "No active recording!");
		return &_frames[_currentFrameIndex];
	}
}