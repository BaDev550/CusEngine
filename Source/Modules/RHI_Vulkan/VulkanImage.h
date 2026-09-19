#pragma once

#include <Graphics/RHI/RHI_Image.h>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace CusEngine::RHI {
	class Vulkan_Image final : public Image {
	public:
		Vulkan_Image(RenderCommands* commands, const ImageDesc& desc);
		Vulkan_Image(VkImage image, VkImageView view, VmaAllocation allocation, const ImageDesc& desc);
		virtual ~Vulkan_Image();

		virtual std::string_view GetObjectDebugName() const override final { return "rhi_object_vulkan_image"; }
		virtual const ImageDesc* GetDesc() const override final { return &_desc; }
		virtual const ImageFormat GetFormat() const override final { return _desc.format; };
		virtual const u32 GetWidth() const noexcept override final { return _desc.width; }
		virtual const u32 GetHeight() const noexcept override final { return _desc.height; }

		[[nodiscard]] VkImage GetVkImage() const { return _image; }
		[[nodiscard]] VkImageView GetVkImageView() const { return _imageView; }
		[[nodiscard]] VmaAllocation GetVmaAllocation() const { return _allocation; }
		[[nodiscard]] VkImageLayout GetVkImageLayout() const;
	private:
		ImageDesc _desc;

		VkImage _image = VK_NULL_HANDLE;
		VkImageView _imageView = VK_NULL_HANDLE;
		VmaAllocation _allocation = VK_NULL_HANDLE;
		u32 _samplerId = 0;

		friend class Vulkan_RenderContext;
		friend class Vulkan_Swapchain;
	};
}