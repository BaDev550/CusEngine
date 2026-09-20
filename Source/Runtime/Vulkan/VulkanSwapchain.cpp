#include <Runtime/Vulkan/VulkanSwapchain.h>
#include <Runtime/Vulkan/VulkanContext.h>
#include <Engine/Core/Memory.h>

namespace CusEngine::RHI {
	VulkanSwapchain::VulkanSwapchain(const SwapchainDesc& desc) : _desc(desc) { }
	VulkanSwapchain::~VulkanSwapchain() {
		Destroy();
	}

	void VulkanSwapchain::Recreate(const SwapchainDesc& desc) {
		VulkanContext* vkContext = GetContext<VulkanContext>();

		_extent.x = desc.width;
		_extent.y = desc.height;
		VkSurfaceCapabilitiesKHR surfaceCaps{};
		Logger::Assert((vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkContext->GetPhysicalDevice(), vkContext->GetSurface(), &surfaceCaps) == VK_SUCCESS), GetObjectDebugName(), "Failed to get surface caps");

		uint32_t requestedImageCount = std::max(2u, surfaceCaps.minImageCount);
		if (surfaceCaps.maxImageCount > 0) requestedImageCount = std::min(requestedImageCount, surfaceCaps.maxImageCount);

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = vkContext->GetSurface();
		createInfo.minImageCount = requestedImageCount;
		createInfo.imageFormat = Utils::GetVkFormat(_colorFormat);
		createInfo.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
		createInfo.imageExtent = VkExtent2D(_extent.x, _extent.y);
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.preTransform = surfaceCaps.currentTransform;
		createInfo.presentMode = _desc.vsync ? VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_IMMEDIATE_KHR;
		Logger::Assert((vkCreateSwapchainKHR(vkContext->GetDevice(), &createInfo, nullptr, &_swapchain) == VK_SUCCESS), GetObjectDebugName(), "Failed to create swapchain");
		Logger::Info(GetObjectDebugName(), "Swapchain created!");

		{
			std::vector<VkImage> vkImages;
			std::vector<VkImageView> vkImageViews;
			vkGetSwapchainImagesKHR(vkContext->GetDevice(), _swapchain, &_imageCount, nullptr);
			vkImages.resize(_imageCount);
			vkImageViews.resize(_imageCount);
			vkGetSwapchainImagesKHR(vkContext->GetDevice(), _swapchain, &_imageCount, vkImages.data());

			_colorAttachments.resize(_imageCount);

			for (uint32_t i = 0; i < _imageCount; i++) {
				ImageDesc attachmentDesc{};
				attachmentDesc.width = _extent.x;
				attachmentDesc.height = _extent.y;
				attachmentDesc.format = _colorFormat;
				attachmentDesc.layout = ImageLayout::Undefined;
				attachmentDesc.usage = ImageUsage::ColorAttachment;
				VulkanImage* vkColorAttachment = Mem::Allocator::Construct<VulkanImage>(attachmentDesc);

				VkImageViewCreateInfo imageViewInfo{};
				imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
				imageViewInfo.image = vkImages[i];
				imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
				imageViewInfo.format = Utils::GetVkFormat(_colorFormat);
				imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				imageViewInfo.subresourceRange.levelCount = 1;
				imageViewInfo.subresourceRange.baseMipLevel = 0;
				imageViewInfo.subresourceRange.layerCount = 1;
				imageViewInfo.subresourceRange.baseArrayLayer = 0;
				Logger::Assert((vkCreateImageView(vkContext->GetDevice(), &imageViewInfo, nullptr, &vkImageViews[i]) == VK_SUCCESS), GetObjectDebugName(), "Failed to create image view for swapchain image");

				vkColorAttachment->_context = _context;
				vkColorAttachment->_image = vkImages[i];
				vkColorAttachment->_imageView = vkImageViews[i];
				vkColorAttachment->_allocation = VK_NULL_HANDLE;

				_colorAttachments[i] = vkColorAttachment;
			}

			{
				ImageDesc depthAttachmentDesc{};
				depthAttachmentDesc.width = _extent.x;
				depthAttachmentDesc.height = _extent.y;
				depthAttachmentDesc.format = _depthFormat;
				depthAttachmentDesc.layout = ImageLayout::Undefined;
				depthAttachmentDesc.usage = ImageUsage::DepthStencilAttachment;

				VulkanImage* depthAttachment = static_cast<VulkanImage*>(_context->CreateImage(depthAttachmentDesc));
				Logger::Assert(depthAttachment, GetObjectDebugName(), "Failed to create swapchain depth image!");

				VkImageView vkDepthImageView;
				VkImageViewCreateInfo depthImgViewCreateInfo{};
				depthImgViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
				depthImgViewCreateInfo.image = depthAttachment->GetImage();
				depthImgViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
				depthImgViewCreateInfo.format = Utils::GetVkFormat(_depthFormat);
				depthImgViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
				depthImgViewCreateInfo.subresourceRange.levelCount = 1;
				depthImgViewCreateInfo.subresourceRange.baseMipLevel = 0;
				depthImgViewCreateInfo.subresourceRange.layerCount = 1;
				depthImgViewCreateInfo.subresourceRange.baseArrayLayer = 0;
				Logger::Assert((vkCreateImageView(GetContext<VulkanContext>()->GetDevice(), &depthImgViewCreateInfo, nullptr, &vkDepthImageView) == VK_SUCCESS), GetObjectDebugName(), "Failed to create image view");

				depthAttachment->_imageView = vkDepthImageView;
				_depthAttachment = std::move(depthAttachment);
			}
		}
	}

	void VulkanSwapchain::Recreate(u32 width, u32 height) {
		_desc.width = width;
		_desc.height = height;
		Recreate(_desc);
	}

	void VulkanSwapchain::Destroy() {
		VulkanContext* vkContext = GetContext<VulkanContext>();

		for (auto& image : _colorAttachments) {
			Mem::Allocator::Destroy(image);
		}
		Mem::Allocator::Destroy(_depthAttachment);
		_colorAttachments.clear();

		vkDestroySwapchainKHR(vkContext->GetDevice(), _swapchain, nullptr);
	}
}