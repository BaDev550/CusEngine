#include <Runtime/Vulkan/VulkanBuffer.h>
#include <Runtime/Vulkan/VulkanUtils.h>
#include <Runtime/Vulkan/VulkanContext.h>

namespace CusEngine::RHI {
	VulkanBuffer::VulkanBuffer(const BufferDesc& desc) : _desc(desc) {}
	VulkanBuffer::~VulkanBuffer() {
		if (_buffer != VK_NULL_HANDLE) vmaDestroyBuffer(GetContext<VulkanContext>()->GetAllocator(), _buffer, _allocation);
	}

	void VulkanBuffer::Write(const void* data, usize size, usize offset) {
		if (size == SIZE_MAX) {
			std::memcpy(_mappedPtr, data, _desc.size);
		}
		else {
			void* oData = ((char*)data + offset);
			std::memcpy(_mappedPtr, oData, size);
		}
	}

	const u64 VulkanBuffer::GetGPUAdress() {
		if ((_desc.usage & BufferUsage::DeviceAddress) != BufferUsage::None && _gpuAddress == 0) {
			VkBufferDeviceAddressInfo bdaInfo{};
			bdaInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
			bdaInfo.buffer = _buffer;

			_gpuAddress = vkGetBufferDeviceAddress(GetContext<VulkanContext>()->GetDevice(), &bdaInfo);
			return _gpuAddress;
		}
		else if (_gpuAddress != 0) {
			return _gpuAddress;
		}
		return 0;
	}
}