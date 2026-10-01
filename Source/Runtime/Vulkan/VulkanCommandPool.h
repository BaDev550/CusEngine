#pragma once

#include <Runtime/RHI/Command/RHICommandPool.h>
#include <Runtime/RHI/Command/RHICommandPoolDesc.h>
#include <Runtime/Vulkan/VulkanCommandBuffer.h>

#include <vulkan/vulkan.h>

namespace Runtime::RHI {
	class VulkanCommandPool : public CommandPool {
	public:
		VulkanCommandPool(Context* context, const CommandPoolDesc& desc);
		virtual ~VulkanCommandPool();

		virtual CommandBuffer* AllocateCommandBuffer(const CommandBufferDesc& desc) override;
		virtual void FreeCommandBuffer(CommandBuffer* commandBuffer) override;

		virtual void Reset() override;
	private:
		CommandPoolDesc _desc;
		VkCommandPool _commandPool = VK_NULL_HANDLE;

		std::list<CommandBuffer*> _allocatedCommandBuffers;
	};
}