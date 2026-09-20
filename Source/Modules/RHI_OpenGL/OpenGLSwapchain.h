#pragma once

#include <Graphics/RHI/RHI_Swapchain.h>
#include "OpenGLUtils.h"
#include "OpenGLImage.h"

namespace CusEngine::RHI {
	class OpenGL_RenderContext;

	class OpenGL_Swapchain final : public Swapchain {
	public:
		OpenGL_Swapchain(OpenGL_RenderContext* context, const SwapchainDesc& desc);
		virtual ~OpenGL_Swapchain();

		virtual void Recreate(const SwapchainDesc& desc) override;
		virtual void Recreate(u32 width, u32 height) override;
		virtual void Destroy() override;

		virtual [[nodiscard]] u32 GetImageCount() const final override { return _imageCount; }
		virtual [[nodiscard]] glm::vec2 GetExtent() const final override { return _extent; }
		virtual [[nodiscard]] __forceinline const ImageFormat GetColorFormat() const final override { return _colorFormat; }
		virtual [[nodiscard]] __forceinline const ImageFormat GetDepthFormat() const final override { return _depthFormat; }

		virtual [[nodiscard]] const std::vector<Mem::Ref<Image>>& GetColorAttachments() const override { return _colorAttachments; };
		virtual [[nodiscard]] const Mem::Ref<Image>& GetDepthAttachment() const override { return _depthAttachment; };
	private:
		OpenGL_RenderContext* _context;
		SwapchainDesc _desc;

		std::vector<Mem::Ref<Image>> _colorAttachments;
		Mem::Ref<Image> _depthAttachment;

		u32 _imageCount = 0;

		glm::vec2 _extent;
		ImageFormat _colorFormat = ImageFormat::RGBA8;
		ImageFormat _depthFormat = ImageFormat::D32_SFLOAT;
	};
}