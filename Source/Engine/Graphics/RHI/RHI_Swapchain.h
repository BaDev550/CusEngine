#pragma once

#include "RHI.h"
#include "Core/Ref.h"

namespace CusEngine::RHI {
	class Swapchain {
	public:
		Swapchain() = default;
		virtual ~Swapchain() = default;

		virtual void Recreate(const SwapchainDesc& desc) = 0;
		virtual void Recreate(u32 width, u32 height) = 0;
		virtual void Destroy() = 0;

		virtual [[nodiscard]] glm::vec2 GetExtent() const = 0;
		virtual [[nodiscard]] u32 GetImageCount() const = 0;

		virtual [[nodiscard]] const std::vector<Mem::Ref<Image>>& GetColorAttachments() const = 0;
		virtual [[nodiscard]] const Mem::Ref<Image>& GetDepthAttachment() const = 0;

		virtual [[nodiscard]] __forceinline const ImageFormat GetColorFormat() const = 0;
		virtual [[nodiscard]] __forceinline const ImageFormat GetDepthFormat() const = 0;
	};
}