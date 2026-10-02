#include "VulkanPipeline.h"
#include "VulkanContext.h"
#include "VulkanCommandBuffer.h"

namespace Runtime::RHI {
	VulkanPipeline::VulkanPipeline(Context* context, const PipelineDesc& desc) : Pipeline(context), _desc(desc) {}
	
	VulkanPipeline::~VulkanPipeline() {
		if (_pipeline != VK_NULL_HANDLE) vkDestroyPipeline(GetOwningRHIContext<VulkanContext>()->GetDevice(), _pipeline, nullptr);
		if (_layout != VK_NULL_HANDLE) vkDestroyPipelineLayout(GetOwningRHIContext<VulkanContext>()->GetDevice(), _layout, nullptr);
	}
	
	void VulkanPipeline::Bind(CommandBuffer* cmd) {
		VulkanCommandBuffer* vkCmd = static_cast<VulkanCommandBuffer*>(cmd);
		vkCmdBindPipeline(vkCmd->GetVkCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline); // TODO(0x): VK_PIPELINE_BIND_POINT_GRAPHICS move it to RHI enum (find a better name for RHI::QueueType)
	}
	
	void VulkanPipeline::PushConstant(CommandBuffer* cmd, void* data, usize size, usize offset) {
		VulkanCommandBuffer* vkCmd = static_cast<VulkanCommandBuffer*>(cmd);
		vkCmdPushConstants(vkCmd->GetVkCommandBuffer(), _layout, VK_SHADER_STAGE_ALL, offset, size, data);
	}
}