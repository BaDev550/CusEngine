#pragma once

#include <Runtime/RHI/Swapchain/RHISwapchain.h>
#include <Runtime/RHI/Swapchain/RHISwapchainDesc.h>

#include <Runtime/Vulkan/VulkanUtils.h>
#include <Runtime/Vulkan/VulkanImage.h>

namespace CusEngine::RHI {
	class VulkanSwapchain final : public Swapchain {
	public:
		VulkanSwapchain(const SwapchainDesc& desc);
		virtual ~VulkanSwapchain();

		virtual std::string_view GetObjectDebugName() const override { return "rhi_object_vulkan_swapchain"; }

		virtual void Recreate(const SwapchainDesc& desc) override;
		virtual void Recreate(u32 width, u32 height) override;
		virtual void Destroy() override;

		[[nodiscard]] VkSwapchainKHR& GetSwapchain() { return _swapchain; }

		virtual [[nodiscard]] u32 GetImageCount() const final override { return _imageCount; }
		virtual [[nodiscard]] glm::vec2 GetExtent() const final override { return _extent; }
		virtual [[nodiscard]] const Format GetColorFormat() const final override { return _colorFormat; }
		virtual [[nodiscard]] const Format GetDepthFormat() const final override { return _depthFormat; }

		virtual [[nodiscard]] const std::vector<Image*>& GetColorAttachments() const override { return _colorAttachments; };
		virtual [[nodiscard]] const Image* GetDepthAttachment() const override { return _depthAttachment; };
		virtual [[nodiscard]] const SwapchainDesc& GetDesc() const override { return _desc; }
	private:
		SwapchainDesc _desc;

		VkSwapchainKHR _swapchain = VK_NULL_HANDLE;

		std::vector<Image*> _colorAttachments;
		Image* _depthAttachment;

		u32 _imageCount = 0;

		glm::vec2 _extent;
		Format _colorFormat = Format::RGBA8;
		Format _depthFormat = Format::D32_SFLOAT;
	};
}