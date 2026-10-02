#include <Runtime/Vulkan/VulkanImage.h>
#include <Runtime/Vulkan/VulkanContext.h>
#include <Runtime/Vulkan/VulkanUtils.h>
#include <Runtime/RHI/Common/RHIUtils.h>

#include <Runtime/Definitions/Logger.h>

namespace Runtime::RHI {
	VulkanImage::VulkanImage(Context* context, const ImageDesc& desc) : Image(context), _desc(desc) { }

	VulkanImage::~VulkanImage() {
		Logger::Info(GetObjectDebugName(), "Destroyed");

		if (_bindessId != u32_max) { GetOwningRHIContext<VulkanContext>()->UnregisterBindlessImage(this); }
		if (_imageView != VK_NULL_HANDLE) vkDestroyImageView(GetOwningRHIContext<VulkanContext>()->GetDevice(), _imageView, nullptr);
		if (_allocation != VK_NULL_HANDLE) vmaDestroyImage(GetOwningRHIContext<VulkanContext>()->GetAllocator(), _image, _allocation);
	}

	void VulkanImage::SetObjectDebugName(const char* name) {
		Object::SetObjectDebugName(name);

		VkDebugUtilsObjectNameInfoEXT info{};
		info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
		info.objectType = VK_OBJECT_TYPE_IMAGE;
		info.objectHandle = (u64)_image;
		info.pObjectName = name;
		GetOwningRHIContext<VulkanContext>()->SetObjectDebugName(&info);
	}

	u32 VulkanImage::GetBindlessIndex() noexcept {
		if (_bindessId == u32_max) {
			_bindessId = GetOwningRHIContext<VulkanContext>()->RegisterBindlessImage(this);
		}
		return _bindessId;
	}

	u32 VulkanImage::GetSamplerIndex() noexcept {
		if (_samplerId == u32_max) {
			_samplerId = GetOwningRHIContext<VulkanContext>()->GetSamplerId(_desc.sampler);
		}
		return _samplerId;
	}

	[[nodiscard]] VkImageLayout VulkanImage::GetImageLayout() const { return Utils::GetVkImageLayout(_layout); }
}