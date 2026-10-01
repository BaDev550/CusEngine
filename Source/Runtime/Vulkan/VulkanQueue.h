#pragma once

#include <Runtime/RHI/Queue/RHIQueue.h>
#include <Runtime/RHI/Queue/RHIQueueDesc.h>
#include <vulkan/vulkan.h>

namespace Runtime::RHI {
	class VulkanQueue : public Queue {
	public:
		VulkanQueue(Context* context, const QueueDesc& desc);
		virtual ~VulkanQueue();

		virtual void Wait() override;
		virtual void Submit(CommandBuffer* commandBuffer, const std::vector<Fence*>& waitFences, const std::vector<Fence*>& signalFences) override;
	private:
		QueueDesc _desc;

		VkQueue _queue = VK_NULL_HANDLE;
		u32 _queueFamilyIndex = 0;

		friend class VulkanContext;
	};
}