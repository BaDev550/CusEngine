#pragma once

#include <Runtime/RHI/Descriptor/DescriptorSet.h>

namespace Runtime::RHI {
	class VulkanContext;

	class VulkanDescriptorSet final : public DescriptorSet {
	public:
		VulkanDescriptorSet(VulkanContext* context, DescriptorSetLayout* layout);
		~VulkanDescriptorSet();

		virtual void WriteImage(u32 binding, u32 arrayIndex, DescriptorType type, Image* image) final {}
		virtual void WriteBuffer(u32 binding, u32 arrayIndex, DescriptorType type, Buffer* buffer) final {}

		[[nodiscard]] virtual const DescriptorSetLayout* GetLayout() const final {}
	private:
		DescriptorSetLayout* _layout;

		VkDescriptorPool _pool;
		VkDescriptorSet _set;
	};
}