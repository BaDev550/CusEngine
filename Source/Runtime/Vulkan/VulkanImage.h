#pragma once

#include <Runtime/RHI/Image/RHIImage.h>
#include <Runtime/RHI/Image/RHIImageDesc.h>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace CusEngine::RHI {
	class VulkanImage final : public Image {
	public:
		VulkanImage(const ImageDesc& desc);
		virtual ~VulkanImage();

		virtual void SetObjectDebugName(const char* name) override final;
		virtual const ImageDesc* GetDesc() const override final { return &_desc; }
		virtual const Format GetFormat() const override final { return _desc.format; };
		virtual const u32 GetWidth() const noexcept override final { return _desc.width; }
		virtual const u32 GetHeight() const noexcept override final { return _desc.height; }
		virtual u32 GetBindlessIndex() noexcept override;

		[[nodiscard]] VkImage GetImage() const { return _image; }
		[[nodiscard]] VkImageView GetImageView() const { return _imageView; }
		[[nodiscard]] VmaAllocation GetAllocation() const { return _allocation; }
		[[nodiscard]] VkImageLayout GetImageLayout() const;
	private:
		ImageDesc _desc;

		VkImage _image = VK_NULL_HANDLE;
		VkImageView _imageView = VK_NULL_HANDLE;
		VmaAllocation _allocation = VK_NULL_HANDLE;
		u32 _bindessId = u32_max;

		friend class VulkanContext;
		friend class VulkanSwapchain;
	};
}