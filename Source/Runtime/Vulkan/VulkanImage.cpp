#include <Runtime/Vulkan/VulkanImage.h>
#include <Runtime/Vulkan/VulkanContext.h>
#include <Runtime/Vulkan/VulkanUtils.h>
#include <Runtime/RHI/Common/RHIUtils.h>

#include <Engine/Core/Logger.h>

namespace CusEngine::RHI {
	VulkanImage::VulkanImage(const ImageDesc& desc) : _desc(desc) {
		Logger::Info(GetObjectDebugName(), "Created {}, {}", desc.width, desc.height);
	}

	VulkanImage::~VulkanImage() {
		Logger::Info(GetObjectDebugName(), "Destroyed");

		if (_imageView != VK_NULL_HANDLE) vkDestroyImageView(GetContext<VulkanContext>()->GetDevice(), _imageView, nullptr);
		if (_allocation != VK_NULL_HANDLE) vmaDestroyImage(GetContext<VulkanContext>()->GetAllocator(), _image, _allocation);
	}

	[[nodiscard]] VkImageLayout VulkanImage::GetImageLayout() const { return Utils::GetVkImageLayout(_desc.layout); }
}