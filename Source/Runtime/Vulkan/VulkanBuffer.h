#pragma once

#include <Runtime/RHI/Buffer/RHIBuffer.h>
#include <Runtime/RHI/Buffer/RHIBufferDesc.h>

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

namespace Runtime::RHI {
	class VulkanBuffer final : public Buffer {
	public:
		VulkanBuffer(Context* context, const BufferDesc& desc);
		virtual ~VulkanBuffer();

		virtual void Write(const void* data, usize size = SIZE_MAX, usize offset = SIZE_MAX) override final;
		virtual const void* GetMappedPtr() const override { return _mappedPtr; }

		virtual const usize GetSize() const noexcept override final { return _desc.size; }
		virtual const BufferDesc* GetDesc() const override { return &_desc; }
		virtual const BufferUsage GetUsage() const override final { return _desc.usage; }
		virtual const MemoryUsage GetMemoryUsage() const override final { return _desc.memoryUsage; }
		virtual const u64 GetGPUAdress() override final;
		[[nodiscard]] VkBuffer GetVkBuffer() const { return _buffer; }
	private:
		BufferDesc _desc;

		void* _mappedPtr = nullptr;

		u64 _gpuAddress = 0;
		usize _allocationSize = SIZE_MAX;
		VkBuffer _buffer = VK_NULL_HANDLE;
		VmaAllocation _allocation;
		
		friend class VulkanContext;
	};
}