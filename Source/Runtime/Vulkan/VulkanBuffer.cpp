#include <Runtime/Vulkan/VulkanBuffer.h>
#include <Runtime/Vulkan/VulkanUtils.h>
#include <Runtime/Vulkan/VulkanContext.h>

namespace Runtime::RHI {
	VulkanBuffer::VulkanBuffer(Context* context, const BufferDesc& desc) : Buffer(context), _desc(desc) {}
	VulkanBuffer::~VulkanBuffer() {
		Logger::Info(GetObjectDebugName(), "Buffer destroyed");
		if (_buffer != VK_NULL_HANDLE) vmaDestroyBuffer(GetOwningRHIContext<VulkanContext>()->GetAllocator(), _buffer, _allocation);
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

			_gpuAddress = vkGetBufferDeviceAddress(GetOwningRHIContext<VulkanContext>()->GetDevice(), &bdaInfo);
			return _gpuAddress;
		}
		else if (_gpuAddress != 0) {
			return _gpuAddress;
		}
		return 0;
	}
}