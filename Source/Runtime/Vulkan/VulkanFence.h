#pragma once

#include <Runtime/RHI/Sync/RHIFence.h>
#include <Runtime/RHI/Sync/RHIFenceDesc.h>

#include <vulkan/vulkan.h>

namespace Runtime::RHI {
	class VulkanFence : public Fence {
	public:
		VulkanFence(const FenceDesc& desc);
		virtual ~VulkanFence();

		virtual void Wait(u64 value) override;

		[[nodiscard]] VkFence GetVkFence() const { return _fence; }
		[[nodiscard]] VkSemaphore GetSemaphore() const { return _semaphore; }
	private:
		FenceDesc _desc;

		VkFence _fence = VK_NULL_HANDLE;
		VkSemaphore _semaphore = VK_NULL_HANDLE;

		friend class VulkanContext;
	};
}