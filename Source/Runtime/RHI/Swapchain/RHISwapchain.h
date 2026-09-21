#pragma once

#include <Engine/Core/Core.h>
#include <Runtime/RHI/Object/RHIObject.h>
#include <Runtime/RHI/Common/RHIFormat.h>
#include <Runtime/RHI/Swapchain/RHISwapchainDesc.h>

#include <glm/glm.hpp>

namespace CusEngine::RHI {
	class Image;

	class Swapchain : public RHIObject {
	public:
		virtual ~Swapchain() = default;

		virtual void Recreate(const SwapchainDesc& desc) = 0;
		virtual void Recreate(u32 width, u32 height) = 0;
		virtual void Destroy() = 0;

		virtual [[nodiscard]] glm::vec2 GetExtent() const = 0;
		virtual [[nodiscard]] u32 GetImageCount() const = 0;

		virtual [[nodiscard]] std::vector<Image*>& GetColorAttachments() = 0;
		virtual [[nodiscard]] Image* GetDepthAttachment() = 0;

		virtual [[nodiscard]] const Format GetColorFormat() const = 0;
		virtual [[nodiscard]] const Format GetDepthFormat() const = 0;
		virtual [[nodiscard]] const SwapchainDesc& GetDesc() const = 0;
	};
}