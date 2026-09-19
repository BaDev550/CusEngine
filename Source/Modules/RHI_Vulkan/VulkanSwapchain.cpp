#include "VulkanSwapchain.h"
#include "VulkanRenderContext.h"

namespace CusEngine::RHI {
	Vulkan_Swapchain::Vulkan_Swapchain(Vulkan_RenderContext* context, const SwapchainDesc& desc) : _context(context) {
		Recreate(desc);
	}

	Vulkan_Swapchain::~Vulkan_Swapchain() {
		Destroy();
	}

	void Vulkan_Swapchain::Recreate(const SwapchainDesc& desc) {
		_extent.x = desc.width;
		_extent.y = desc.height;
		VkSurfaceCapabilitiesKHR surfaceCaps{};
		Logger::Assert((vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_context->GetVkPhysicalDevice(), _context->GetVkSurface(), &surfaceCaps) == VK_SUCCESS), "Vulkan swapchain", "Failed to get surface caps");

		uint32_t requestedImageCount = std::max(2u, surfaceCaps.minImageCount);
		if (surfaceCaps.maxImageCount > 0)
			requestedImageCount = std::min(requestedImageCount, surfaceCaps.maxImageCount);

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = _context->GetVkSurface();
		createInfo.minImageCount = requestedImageCount;
		createInfo.imageFormat = Utils::GetVkFormat(_colorFormat);
		createInfo.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
		createInfo.imageExtent = VkExtent2D(_extent.x, _extent.y);
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.preTransform = surfaceCaps.currentTransform;
		createInfo.presentMode = _desc.vsync ? VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_IMMEDIATE_KHR;
		Logger::Assert((vkCreateSwapchainKHR(_context->GetVkDeviceHandle(), &createInfo, nullptr, &_swapchain) == VK_SUCCESS), "Vulkan swapchain", "Failed to create swapchain");
		Logger::Info("Vulkan Swapchain", "Swapchain created!");

		{
			std::vector<VkImage> _RAWimages;
			std::vector<VkImageView> _RAWimageViews;

			uint32_t imageCount = 0;
			vkGetSwapchainImagesKHR(_context->GetVkDeviceHandle(), _swapchain, &imageCount, nullptr);
			_RAWimages.resize(imageCount);
			vkGetSwapchainImagesKHR(_context->GetVkDeviceHandle(), _swapchain, &imageCount, _RAWimages.data());
			_RAWimageViews.resize(imageCount);
			_colorAttachments.resize(imageCount);
			_imageCount = imageCount;

			for (uint32_t i = 0; i < imageCount; i++) {
				VkImageViewCreateInfo imageViewInfo{};
				imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
				imageViewInfo.image = _RAWimages[i];
				imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
				imageViewInfo.format = Utils::GetVkFormat(_colorFormat);
				imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				imageViewInfo.subresourceRange.levelCount = 1;
				imageViewInfo.subresourceRange.baseMipLevel = 0;
				imageViewInfo.subresourceRange.layerCount = 1;
				imageViewInfo.subresourceRange.baseArrayLayer = 0;
				Logger::Assert((vkCreateImageView(_context->GetVkDeviceHandle(), &imageViewInfo, nullptr, &_RAWimageViews[i]) == VK_SUCCESS), "Vulkan swapchain", "Failed to create image view for swapchain image");

				ImageDesc swapchainColorAttachmentDesc{};
				swapchainColorAttachmentDesc.width = _extent.x;
				swapchainColorAttachmentDesc.height = _extent.y;
				swapchainColorAttachmentDesc.format = _colorFormat;
				swapchainColorAttachmentDesc.layout = ImageLayout::Undefined;
				swapchainColorAttachmentDesc.usage = ImageUsage::ColorAttachment;
				_colorAttachments[i] = Mem::Ref<Vulkan_Image>::Create(
					_RAWimages[i],
					_RAWimageViews[i],
					VK_NULL_HANDLE,
					swapchainColorAttachmentDesc
				);
			}

			VkImage _RAWdepthImage;
			VmaAllocation _RAWdepthImageAllocation;
			VkImageView _RAWdepthImageView;
			Logger::Assert((
				_context->CreateImage(
					_extent.x, 
					_extent.y, 
					Utils::GetVkFormat(_depthFormat), 
					VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 
					&_RAWdepthImage, 
					&_RAWdepthImageAllocation
				) == VK_SUCCESS), "Vulkan swapchain", "Failed to create swapchain depth image!");

			VkImageViewCreateInfo depthImgViewCreateInfo{};
			depthImgViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			depthImgViewCreateInfo.image = _RAWdepthImage;
			depthImgViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			depthImgViewCreateInfo.format = Utils::GetVkFormat(_depthFormat);
			depthImgViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			depthImgViewCreateInfo.subresourceRange.levelCount = 1;
			depthImgViewCreateInfo.subresourceRange.baseMipLevel = 0;
			depthImgViewCreateInfo.subresourceRange.layerCount = 1;
			depthImgViewCreateInfo.subresourceRange.baseArrayLayer = 0;
			Logger::Assert((vkCreateImageView(_context->GetVkDeviceHandle(), &depthImgViewCreateInfo, nullptr, &_RAWdepthImageView) == VK_SUCCESS), "Vulkan swapchain", "Failed to create image view");

			ImageDesc swapchainDepthAttachmentDesc{};
			swapchainDepthAttachmentDesc.width = _extent.x;
			swapchainDepthAttachmentDesc.height = _extent.y;
			swapchainDepthAttachmentDesc.format = _depthFormat;
			swapchainDepthAttachmentDesc.layout = ImageLayout::Undefined;
			swapchainDepthAttachmentDesc.usage = ImageUsage::DepthStencilAttachment;
			_depthAttachment = Mem::Ref<Vulkan_Image>::Create(
				_RAWdepthImage,
				_RAWdepthImageView,
				_RAWdepthImageAllocation,
				swapchainDepthAttachmentDesc
			);
		}
	}

	void Vulkan_Swapchain::Recreate(u32 width, u32 height) {
		_desc.width = width;
		_desc.height = height;
		Recreate(_desc);
	}

	void Vulkan_Swapchain::Destroy() {
		for (auto& image : _colorAttachments) {
			Mem::Ref<Vulkan_Image> vkImage = image.AsStatic<Vulkan_Image>();

			vkDestroyImageView(_context->GetVkDeviceHandle(), vkImage->GetVkImageView(), nullptr);
			vkImage->_imageView = VK_NULL_HANDLE;
		}
		_colorAttachments.clear();

		Mem::Ref<Vulkan_Image> vkDepthImage = _depthAttachment.AsStatic<Vulkan_Image>();

		vmaDestroyImage(_context->GetAllocator(), vkDepthImage->GetVkImage(), vkDepthImage->GetVmaAllocation());
		vkDestroyImageView(_context->GetVkDeviceHandle(), vkDepthImage->GetVkImageView(), nullptr);
		vkDepthImage->_image = VK_NULL_HANDLE;
		vkDepthImage->_imageView = VK_NULL_HANDLE;
		vkDepthImage->_allocation = VK_NULL_HANDLE;

		vkDestroySwapchainKHR(_context->GetVkDeviceHandle(), _swapchain, nullptr);
	}
}