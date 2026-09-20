#include "OpenGLRenderCommands.h"

#include "OpenGLBuffer.h"
#include "OpenGLImage.h"

#include <Core/Engine.h>
#include <Graphics/Framebuffer.h>
#include <Window/WindowSubsystem.h>

namespace CusEngine::RHI {
	OpenGL_RenderCommands::OpenGL_RenderCommands(OpenGL_RenderContext* context, OpenGL_Swapchain* swapchain) : _context(context), _swapchain(swapchain) {

	}

	OpenGL_RenderCommands::~OpenGL_RenderCommands() {

	}

	void OpenGL_RenderCommands::BeginFrame() {

	}

	void OpenGL_RenderCommands::EndFrame() {

	}

	void OpenGL_RenderCommands::Submit(CommandFunc func) {  }

	void OpenGL_RenderCommands::Track(const Mem::Ref<RenderObject>& object) { }

	void OpenGL_RenderCommands::Wait() {

	}

	void OpenGL_RenderCommands::BeginDynamicRendering(std::vector<Mem::Ref<Image>> colorAttachments, Mem::Ref<Image> depthAttachment, glm::vec2 extent, glm::vec4 clearColor) {

	}

	void OpenGL_RenderCommands::EndDynamicRendering() {

	}

	void OpenGL_RenderCommands::TransitionImageLayout(Image* image, ImageLayout newLayout) {

	}

	void OpenGL_RenderCommands::CopyBuffer(Buffer* srcBuffer, Buffer* dstBuffer, size_t size) {

	}

	void OpenGL_RenderCommands::CopyBufferToImage(Buffer* buffer, Image* image, ImageLayout layout, u32 width, u32 height) {

	}

	u32 OpenGL_RenderCommands::RegisterBindlessImage(const Mem::Ref<Image>& image) {
		u32 index = _bindlessImages.size();
		_bindlessImages.push_back(image);
		return index;
	}

	uint32_t OpenGL_RenderCommands::GetImageIndex() const noexcept { return 0; }

	Swapchain* OpenGL_RenderCommands::GetTargetSwapchain() const { return _swapchain; }
	RenderContext* OpenGL_RenderCommands::GetTargetRenderContext() const { return _context; }
}