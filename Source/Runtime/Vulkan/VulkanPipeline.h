#pragma once

#include <Runtime/RHI/Pipeline/RHIPipeline.h>
#include <vulkan/vulkan.h>

namespace Runtime::RHI {
	class VulkanPipeline final : public Pipeline {
	public:
		VulkanPipeline(Context* context, const PipelineDesc& desc);
		virtual ~VulkanPipeline();

		virtual void Bind(CommandBuffer* cmd) override;
		virtual void PushConstant(CommandBuffer* cmd, void* data, usize size, usize offset = 0) override;

		[[nodiscard]] virtual const PipelineDesc& GetDesc() const override { return _desc; }
		[[nodiscard]] VkPipeline GetVkPipeline() const { return _pipeline; }
		[[nodiscard]] VkPipelineLayout GetVkLayout() const { return _layout; }
	private:
		PipelineDesc _desc;

		VkPipeline _pipeline;
		VkPipelineLayout _layout;

		friend class VulkanContext;
	};
}