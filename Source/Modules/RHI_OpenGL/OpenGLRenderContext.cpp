#include "OpenGLRenderContext.h"
#include "OpenGLUtils.h"

#include <Core/Logger.h>
#include <Graphics/Texture2D.h>

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

namespace CusEngine::RHI {
	OpenGL_RenderContext::OpenGL_RenderContext(const RenderContextDesc& desc) : _desc(desc) {
		Logger::Info("OpenGL Render Context", "Created!");
	}

	OpenGL_RenderContext::~OpenGL_RenderContext() { }

	void OpenGL_RenderContext::Shutdown() {

	}

	void OpenGL_RenderContext::WaitDeviceIdle()
	{}
}