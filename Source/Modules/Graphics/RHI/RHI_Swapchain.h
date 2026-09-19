#pragma once

#include "RHI.h"

namespace Graphics {
	class RHI_Swapchain {
	public:
		RHI_Swapchain() = default;
		virtual ~RHI_Swapchain() = default;

		virtual void Recreate(const RHI_SwapchainDesc& desc) = 0;
		virtual void Destroy() = 0;

		virtual [[nodiscard]] u32 GetImageCount() const = 0;
		virtual [[nodiscard]] glm::vec2 GetExtent() const = 0;
		virtual [[nodiscard]] const std::vector<Memory::Ref<RHI_Image>>& GetColorAttachments() const = 0;
		virtual [[nodiscard]] const Memory::Ref<RHI_Image>& GetDepthAttachment() const = 0;
		virtual [[nodiscard]] RHI_SwapchainHandle GetNativeHandle() const = 0;
		virtual [[nodiscard]] __forceinline const RHI_Format GetColorFormat() const = 0;
		virtual [[nodiscard]] __forceinline const RHI_Format GetDepthFormat() const = 0;
	};
}