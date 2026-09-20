#include <Runtime/Vulkan/VulkanImage.h>
#include <Runtime/Vulkan/VulkanContext.h>
#include <Runtime/Vulkan/VulkanUtils.h>
#include <Runtime/RHI/Common/RHIUtils.h>

#include <Engine/Core/Logger.h>

namespace CusEngine::RHI {
	VulkanImage::VulkanImage(const ImageDesc& desc) : _desc(desc) {
		//GetContext<VulkanContext>()->CreateImage(
		//	_desc.width,
		//	_desc.height,
		//	Utils::GetVkFormat(_desc.format),
		//	Utils::GetVkImageUsage(_desc.usage),
		//	&_image,
		//	&_allocation,
		//	Utils::GetVkImageTiling(_desc.tileMode)
		//);
	}

	VulkanImage::VulkanImage(VkImage image, VkImageView view, VmaAllocation allocation, const ImageDesc& desc) :
		_image(image), _imageView(view), _allocation(allocation), _desc(desc) {}

	VulkanImage::~VulkanImage() {
		Logger::Info("Vulkan_Image", "Destroyed Vulkan image");

		if (_imageView != VK_NULL_HANDLE) vkDestroyImageView(GetContext<VulkanContext>()->GetDevice(), _imageView, nullptr);
		if (_allocation != VK_NULL_HANDLE) vmaDestroyImage(GetContext<VulkanContext>()->GetAllocator(), _image, _allocation);
	}

	[[nodiscard]] VkImageLayout VulkanImage::GetImageLayout() const { return Utils::GetVkImageLayout(_desc.layout); }
}