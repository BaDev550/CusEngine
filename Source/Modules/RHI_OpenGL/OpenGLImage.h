#pragma once

#include <Graphics/RHI/RHI_Image.h>


namespace CusEngine::RHI {
	class OpenGL_Image final : public Image {
	public:
		OpenGL_Image(RenderCommands* commands, const ImageDesc& desc);
		virtual ~OpenGL_Image();

		virtual std::string_view GetObjectDebugName() const override final { return "rhi_object_opengl_image"; }
		virtual const ImageDesc* GetDesc() const override final { return &_desc; }
		virtual const ImageFormat GetFormat() const override final { return _desc.format; };
		virtual const u32 GetWidth() const noexcept override final { return _desc.width; }
		virtual const u32 GetHeight() const noexcept override final { return _desc.height; }
	private:
		ImageDesc _desc;

		u32 _samplerId = 0;

		friend class OpenGL_RenderContext;
		friend class OpenGL_Swapchain;
	};
}