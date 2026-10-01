#pragma once

#include <Runtime/RHI/Sync/RHIFence.h>
#include <Runtime/RHI/Sync/RHIFenceDesc.h>

#include <vulkan/vulkan.h>

namespace Runtime::RHI {
	class VulkanFence : public Fence {
	public:
		VulkanFence(Context* context, const FenceDesc& desc);
		virtual ~VulkanFence();

		virtual void Wait(u64 value) override;
		virtual void Signal(u64 value) override { _currentValue = value; }
		virtual u64 GetCurrentValue() override { return _currentValue; }

		[[nodiscard]] VkFence GetVkFence() const { return _fence; }
		[[nodiscard]] VkSemaphore GetSemaphore() const { return _semaphore; }
	private:
		FenceDesc _desc;

		VkFence _fence = VK_NULL_HANDLE;
		VkSemaphore _semaphore = VK_NULL_HANDLE;
		u64 _currentValue = 0;

		friend class VulkanContext;
	};
}