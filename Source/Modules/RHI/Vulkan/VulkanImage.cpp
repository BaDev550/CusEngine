#include "VulkanImage.h"

#include "Core/Logger.h"

#include "Graphics/RHI/RHI_Utils.h"

#include "VulkanRenderCommands.h"
#include "VulkanUtils.h"

namespace Graphics {
	Vulkan_Image::Vulkan_Image(RHI_RenderCommands* commands, const RHI_ImageDesc& desc)
		: RHI_Image(commands), _desc(desc) {

		Logger::Info("Vulkan_Image", "Queued Vulkan image with width: {}, height: {}, format: {}, usage: {}, tile mode: {}",
			_desc.Width, _desc.Height, Utils::RHI_FormatToString(_desc.Format), Utils::RHI_ImageUsageToString(_desc.Usage), Utils::RHI_ImageTileModeToString(_desc.TileMode));

		GetRenderCommands<Vulkan_RenderCommands>()->GetVkContext()->CreateImage(
			_desc.Width,
			_desc.Height,
			Utils::RHI_GetVkFormat(_desc.Format),
			Utils::RHI_GetVkImageUsage(_desc.Usage),
			&_image,
			&_allocation,
			Utils::RHI_GetVkImageTiling(_desc.TileMode)
		);
	}

	Vulkan_Image::Vulkan_Image(VkImage image, VkImageView view, VmaAllocation allocation, const RHI_ImageDesc& desc) : 
		_image(image), _imageView(view), _allocation(allocation), _desc(desc) {}

	Vulkan_Image::~Vulkan_Image() {
		Logger::Info("Vulkan_Image", "Destroyed Vulkan image");

		if (_imageView != VK_NULL_HANDLE)
			vkDestroyImageView(GetRenderCommands<Vulkan_RenderCommands>()->GetVkContext()->GetVkDeviceHandle(), _imageView, nullptr);
		if (_allocation != VK_NULL_HANDLE)
			vmaDestroyImage(GetRenderCommands<Vulkan_RenderCommands>()->GetVkContext()->GetAllocator(), _image, _allocation);
	}

	[[nodiscard]] VkImageLayout Vulkan_Image::GetVkImageLayout() const { return Utils::RHI_GetVkImageLayout(_desc.Layout); }
}