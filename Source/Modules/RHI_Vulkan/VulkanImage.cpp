#include "VulkanImage.h"

#include <Core/Logger.h>
#include <Graphics/RHI/RHI_Utils.h>

#include "VulkanRenderCommands.h"
#include "VulkanUtils.h"

namespace CusEngine::RHI {
	Vulkan_Image::Vulkan_Image(RenderCommands* commands, const ImageDesc& desc) : Image(commands), _desc(desc) {
		Logger::Info("Vulkan_Image", "Queued Vulkan image with width: {}, height: {}, format: {}, usage: {}, tile mode: {}",
			_desc.width, _desc.height, Utils::FormatToString(_desc.format), Utils::ImageUsageToString(_desc.usage), Utils::ImageTileModeToString(_desc.tileMode));

		GetRenderCommands<Vulkan_RenderCommands>()->GetVkContext()->CreateImage(
			_desc.width,
			_desc.height,
			Utils::GetVkFormat(_desc.format),
			Utils::GetVkImageUsage(_desc.usage),
			&_image,
			&_allocation,
			Utils::GetVkImageTiling(_desc.tileMode)
		);
	}

	Vulkan_Image::Vulkan_Image(VkImage image, VkImageView view, VmaAllocation allocation, const ImageDesc& desc) : 
		_image(image), _imageView(view), _allocation(allocation), _desc(desc) {}

	Vulkan_Image::~Vulkan_Image() {
		Logger::Info("Vulkan_Image", "Destroyed Vulkan image");

		if (_imageView != VK_NULL_HANDLE)
			vkDestroyImageView(GetRenderCommands<Vulkan_RenderCommands>()->GetVkContext()->GetVkDeviceHandle(), _imageView, nullptr);
		if (_allocation != VK_NULL_HANDLE)
			vmaDestroyImage(GetRenderCommands<Vulkan_RenderCommands>()->GetVkContext()->GetAllocator(), _image, _allocation);
	}

	[[nodiscard]] VkImageLayout Vulkan_Image::GetVkImageLayout() const { return Utils::GetVkImageLayout(_desc.layout); }
}