#include "VulkanFence.h"

#include <Runtime/Vulkan/VulkanContext.h>

namespace Runtime::RHI {
	VulkanFence::VulkanFence(Context* context, const FenceDesc& desc) : Fence(context), _desc(desc) { }

	VulkanFence::~VulkanFence() {
		if (_semaphore != VK_NULL_HANDLE) { vkDestroySemaphore(GetOwningRHIContext<VulkanContext>()->GetDevice(), _semaphore, nullptr); }
		if (_fence != VK_NULL_HANDLE) { vkDestroyFence(GetOwningRHIContext<VulkanContext>()->GetDevice(), _fence, nullptr); }
	}

	void VulkanFence::Wait(u64 value) {
		VkSemaphoreWaitInfo waitInfo{};
		waitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
		waitInfo.pValues = &value;
		waitInfo.semaphoreCount = 1;
		waitInfo.pSemaphores = &_semaphore;
		
		vkWaitSemaphores(GetOwningRHIContext<VulkanContext>()->GetDevice(), &waitInfo, UINT64_MAX);
	}
}