#pragma once

#include "Graphics/RHI/RHI_Swapchain.h"
#include "VulkanUtils.h"
#include "VulkanImage.h"

namespace Graphics {
	class Vulkan_RenderContext;
	class Vulkan_Swapchain final : public RHI_Swapchain {
	public:
		Vulkan_Swapchain(Vulkan_RenderContext* context, const RHI_SwapchainDesc& desc);
		virtual ~Vulkan_Swapchain();

		virtual void Recreate(const RHI_SwapchainDesc& desc) override;
		virtual void Destroy() override;

		virtual [[nodiscard]] u32 GetImageCount() const final override { return _imageCount; }
		virtual [[nodiscard]] glm::vec2 GetExtent() const final override { return _extent; }
		[[nodiscard]] RHI_SwapchainHandle GetNativeHandle() const final override { return reinterpret_cast<RHI_SwapchainHandle>(_swapchain); }
		[[nodiscard]] VkSwapchainKHR& GetVkSwapchainHandle() { return _swapchain; }
		virtual [[nodiscard]] __forceinline const RHI_Format GetColorFormat() const final override { return _colorFormat; }
		virtual [[nodiscard]] __forceinline const RHI_Format GetDepthFormat() const final override { return _depthFormat; }

		virtual [[nodiscard]] const std::vector<Memory::Ref<RHI_Image>>& GetColorAttachments() const override { return _colorAttachments; };
		virtual [[nodiscard]] const Memory::Ref<RHI_Image>& GetDepthAttachment() const override { return _depthAttachment; };
	private:
		Vulkan_RenderContext* _context;
		RHI_SwapchainDesc _desc;

		VkSwapchainKHR _swapchain = VK_NULL_HANDLE;

		std::vector<Memory::Ref<RHI_Image>> _colorAttachments;
		Memory::Ref<RHI_Image> _depthAttachment;

		u32 _imageCount = 0;

		glm::vec2 _extent;
		RHI_Format _colorFormat = RHI_Format::RGBA8;
		RHI_Format _depthFormat = RHI_Format::D32_SFLOAT;
	};
}