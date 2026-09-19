#pragma once

#include "Graphics/RHI/RHI_Buffer.h"
#include "VulkanRenderContext.h"

namespace Graphics {
	class Vulkan_Buffer final : public RHI_Buffer {
	public:
		Vulkan_Buffer(RHI_RenderCommands* commands, const RHI_BufferDesc& desc);
		virtual ~Vulkan_Buffer();

		virtual void Write(const void* data, size_t size = SIZE_MAX, size_t offset = SIZE_MAX) override final;
		virtual const void* GetMappedPtr() const override { return _mappedPtr; }

		virtual std::string_view GetObjectDebugName() const override { return "rhi_object_vulkan_buffer"; };
		virtual RHI_BufferHandle GetNativeHandle() const override { return reinterpret_cast<RHI_BufferHandle>(_buffer); }
		virtual const size_t GetSize() const noexcept override final { return _desc.Size; }
		virtual const RHI_BufferDesc* GetDesc() const override { return &_desc; }
		virtual const RHI_BufferUsage GetUsage() const override final { return _desc.Usage; }
		virtual const RHI_MemoryUsage GetMemoryUsage() const override final { return _desc.MemoryUsage; }
		virtual const u64 GetGPUAdress() override final;
		[[nodiscard]] VkBuffer GetVkBuffer() const { return _buffer; }
	private:
		RHI_BufferDesc _desc;

		void* _mappedPtr = nullptr;
		size_t _allocationSize = SIZE_MAX;
		VkBuffer _buffer = VK_NULL_HANDLE;
		VmaAllocation _allocation;
		u64 _gpuAddress = 0;
	};
}