#include "OpenGLSwapchain.h"
#include "OpenGLRenderContext.h"

namespace CusEngine::RHI {
	OpenGL_Swapchain::OpenGL_Swapchain(OpenGL_RenderContext* context, const SwapchainDesc& desc) : _context(context) {
		Recreate(desc);
	}

	OpenGL_Swapchain::~OpenGL_Swapchain() {
		Destroy();
	}

	void OpenGL_Swapchain::Recreate(const SwapchainDesc& desc) {
		Logger::Info("OpenGL Swapchain", "Created!");
	}

	void OpenGL_Swapchain::Recreate(u32 width, u32 height) {

	}

	void OpenGL_Swapchain::Destroy() {

	}
}