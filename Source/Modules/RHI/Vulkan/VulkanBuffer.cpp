#include "VulkanBuffer.h"
#include "VulkanRenderCommands.h"
#include "VulkanUtils.h"

namespace Graphics {
	Vulkan_Buffer::Vulkan_Buffer(RHI_RenderCommands* commands, const RHI_BufferDesc& desc) : RHI_Buffer(commands), _desc(desc) {
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = _desc.Size;
		bufferInfo.usage = Utils::RHI_GetVkBufferUsage(_desc.Usage);
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = Utils::RHI_GetVkMemoryUsage(_desc.MemoryUsage);
		allocInfo.flags = Utils::RHI_GetVkAllocationFlags(_desc.AllocationFlags);
		VmaAllocationInfo info{};
		Logger::Assert((
			vmaCreateBuffer(
				GetRenderCommands<Vulkan_RenderCommands>()->GetVkContext()->GetAllocator(), 
				&bufferInfo, 
				&allocInfo, 
				&_buffer, 
				&_allocation, 
				&info) == VK_SUCCESS), "Vulkan buffer", "Failed to create buffer!");

		_mappedPtr = info.pMappedData;
		_allocationSize = info.size;
	}

	Vulkan_Buffer::~Vulkan_Buffer() {
		if (_buffer != VK_NULL_HANDLE) {
			vmaDestroyBuffer(GetRenderCommands<Vulkan_RenderCommands>()->GetVkContext()->GetAllocator(), _buffer, _allocation);
		}
	}

	void Vulkan_Buffer::Write(const void* data, size_t size, size_t offset) {
		if (size == SIZE_MAX) {
			std::memcpy(_mappedPtr, data, _desc.Size);
		}
		else {
			void* oData = ((char*)data + offset);
			std::memcpy(_mappedPtr, oData, size);
		}
	}

	const u64 Vulkan_Buffer::GetGPUAdress()
	{
		if ((_desc.Usage & RHI_BufferUsage::DeviceAddress) != RHI_BufferUsage::None && _gpuAddress == 0) {
			VkBufferDeviceAddressInfo bdaInfo{};
			bdaInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
			bdaInfo.buffer = _buffer;

			_gpuAddress = vkGetBufferDeviceAddress(GetRenderCommands<Vulkan_RenderCommands>()->GetVkContext()->GetVkDeviceHandle(), &bdaInfo);
			return _gpuAddress;
		}
		else if (_gpuAddress != 0) {
			return _gpuAddress;
		}
		return 0;
	}
}