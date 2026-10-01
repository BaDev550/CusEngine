#include "VulkanCommandPool.h"
#include "VulkanContext.h"
#include "VulkanCommandBuffer.h"
#include "VulkanUtils.h"

#include <Runtime/Memory/Memory.h>

namespace Runtime::RHI {
    VulkanCommandPool::VulkanCommandPool(Context* context, const CommandPoolDesc& desc) : CommandPool(context), _desc(desc) {}

    VulkanCommandPool::~VulkanCommandPool() {
		if (_commandPool != VK_NULL_HANDLE) {
			vkDestroyCommandPool(GetOwningRHIContext<VulkanContext>()->GetDevice(), _commandPool, nullptr);
		}
    }

    CommandBuffer* VulkanCommandPool::AllocateCommandBuffer(const CommandBufferDesc& desc) {
		VulkanCommandBuffer* commandBuffer = Mem::Allocator::Construct<VulkanCommandBuffer>(GetOwningRHIContext<VulkanContext>(), desc);

        VkCommandBufferAllocateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        info.commandPool = _commandPool;
        info.level = Utils::GetVkCommandBufferLevel(desc.type);
        info.commandBufferCount = 1;

        VkResult result = vkAllocateCommandBuffers(GetOwningRHIContext<VulkanContext>()->GetDevice(), &info, &commandBuffer->_commandBuffer);
        if (result != VK_SUCCESS) {
            return nullptr;
        }
		return commandBuffer;
    }

    void VulkanCommandPool::FreeCommandBuffer(CommandBuffer* commandBuffer) {
		VulkanCommandBuffer* vkCommandBuffer = static_cast<VulkanCommandBuffer*>(commandBuffer);
		VkCommandBuffer vkCommandBufferHandle = vkCommandBuffer->GetVkCommandBuffer();

        vkFreeCommandBuffers(GetOwningRHIContext<VulkanContext>()->GetDevice(), _commandPool, 1, &vkCommandBufferHandle);
    }

    void VulkanCommandPool::Reset() {
        vkResetCommandPool(GetOwningRHIContext<VulkanContext>()->GetDevice(), _commandPool, 0);
    }
}