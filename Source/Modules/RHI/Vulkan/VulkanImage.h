#pragma once

#include "Graphics/RHI/RHI_Image.h"

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Graphics {
	class Vulkan_Image final : public RHI_Image {
	public:
		Vulkan_Image(RHI_RenderCommands* commands, const RHI_ImageDesc& desc);
		Vulkan_Image(VkImage image, VkImageView view, VmaAllocation allocation, const RHI_ImageDesc& desc);
		virtual ~Vulkan_Image();

		virtual std::string_view GetObjectDebugName() const override final { return "rhi_object_vulkan_image"; }
		virtual RHI_ImageHandle GetNativeHandle() const override final { return reinterpret_cast<RHI_ImageHandle>(_image); }
		virtual const RHI_ImageDesc* GetDesc() const override final { return &_desc; }
		virtual const RHI_Format GetFormat() const override final { return _desc.Format; };
		virtual const u32 GetWidth() const noexcept override final { return _desc.Width; }
		virtual const u32 GetHeight() const noexcept override final { return _desc.Height; }
		virtual const u32 GetSamplerId() const noexcept override; // TEMP
		virtual void SetSamplerId(u32 id) override {
			_samplerId = id;
		}

		[[nodiscard]] VkImage GetVkImage() const { return _image; }
		[[nodiscard]] VkImageView GetVkImageView() const { return _imageView; }
		[[nodiscard]] VmaAllocation GetVmaAllocation() const { return _allocation; }
		[[nodiscard]] VkImageLayout GetVkImageLayout() const;
	private:
		RHI_ImageDesc _desc;

		VkImage _image = VK_NULL_HANDLE;
		VkImageView _imageView = VK_NULL_HANDLE;
		VmaAllocation _allocation = VK_NULL_HANDLE;
		u32 _samplerId = 0;

		friend class Vulkan_RenderContext;
		friend class Vulkan_Swapchain;
	};
}