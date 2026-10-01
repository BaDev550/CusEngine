#include "VulkanQueue.h"
#include "VulkanFence.h"
#include "VulkanCommandBuffer.h"
#include "VulkanSwapchain.h"

namespace Runtime::RHI {
	VulkanQueue::VulkanQueue(Context* context, const QueueDesc& desc) : Queue(context), _desc(desc) {}

	VulkanQueue::~VulkanQueue() { }

	void VulkanQueue::Wait() {
		vkQueueWaitIdle(_queue);
	}

	void VulkanQueue::Submit(CommandBuffer* commandBuffer, const std::vector<Fence*>& waitFences, const std::vector<Fence*>& signalFences) {
		if (_desc.type == QueueType::Graphics) {
			VulkanCommandBuffer* vkCmdBuffer = static_cast<VulkanCommandBuffer*>(commandBuffer);

			VkCommandBufferSubmitInfo cmdSubmitInfo{};
			cmdSubmitInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
			cmdSubmitInfo.commandBuffer = vkCmdBuffer->GetVkCommandBuffer();

			std::vector<VkSemaphoreSubmitInfo> semaphoreWaits;
			semaphoreWaits.reserve(waitFences.size());
			for (auto& fence : waitFences) {
				VulkanFence* vkFence = static_cast<VulkanFence*>(fence);
				VkSemaphoreSubmitInfo waitInfo{};
				waitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
				waitInfo.semaphore = vkFence->GetSemaphore();
				waitInfo.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT; // TODO(0x): Make this configurable
				semaphoreWaits.push_back(waitInfo);
			}

			std::vector<VkSemaphoreSubmitInfo> semaphoreSignals;
			semaphoreSignals.reserve(waitFences.size());
			for (auto& fence : signalFences) {
				VulkanFence* vkFence = static_cast<VulkanFence*>(fence);
				VkSemaphoreSubmitInfo signalInfo{};
				signalInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
				signalInfo.semaphore = vkFence->GetSemaphore();
				signalInfo.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
				signalInfo.value = vkFence->GetCurrentValue();
				semaphoreSignals.push_back(signalInfo);
			}

			VkSubmitInfo2 submitInfo{};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
			submitInfo.waitSemaphoreInfoCount = static_cast<u32>(semaphoreWaits.size());
			submitInfo.pWaitSemaphoreInfos = semaphoreWaits.data();
			submitInfo.commandBufferInfoCount = 1;
			submitInfo.pCommandBufferInfos = &cmdSubmitInfo;
			submitInfo.signalSemaphoreInfoCount = static_cast<u32>(semaphoreSignals.size());
			submitInfo.pSignalSemaphoreInfos = semaphoreSignals.data();

			vkQueueSubmit2(_queue, 1, &submitInfo, VK_NULL_HANDLE);
		}
	}

	void VulkanQueue::Present(Swapchain* swapchain, u32 imageIndex, const std::vector<Fence*>& waitFences) {
		VulkanSwapchain* vkSwapchain = static_cast<VulkanSwapchain*>(swapchain);

		std::vector<VkSemaphore> waitSemaphores;
		for (auto& fence : waitFences) {
			VulkanFence* vkFence = static_cast<VulkanFence*>(fence);
			waitSemaphores.push_back(vkFence->GetSemaphore());
		}

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = static_cast<u32>(waitSemaphores.size());
		presentInfo.pWaitSemaphores = waitSemaphores.data();
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &vkSwapchain->GetSwapchain();
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr;
		vkQueuePresentKHR(_queue, &presentInfo);
	}

}