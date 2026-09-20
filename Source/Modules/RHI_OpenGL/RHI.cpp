#include <Graphics/RHI/RHI_RenderContext.h>
#include <Graphics/RHI/RHI_Image.h>

#include "OpenGLRenderContext.h"
#include "OpenGLRenderCommands.h"
#include "OpenGLSwapchain.h"
#include "OpenGLBuffer.h"
#include "OpenGLImage.h"

#include <vector>

extern "C" {
#define CHECK_DEPENDEND(depended) Logger::Assert(depended, "RHI", "Invalid context")
#define DESTROY_OBJECT_CHECKED(obj) if (obj) delete obj

	namespace CusEngine::RHI {
		ENGINE_API [[nodiscard]] RenderContext* CreateRenderContext(const RenderContextDesc& desc) { 
			return new OpenGL_RenderContext(desc); 
		}

		ENGINE_API [[nodiscard]] void DestroyRenderContext(RenderContext* context) {
			if (context) {
				context->Shutdown();
				delete context;
			}
		}

		ENGINE_API [[nodiscard]] Swapchain* CreateSwapchain(RenderContext* context, const SwapchainDesc& desc) {
			CHECK_DEPENDEND(context);
			OpenGL_RenderContext* glRenderContext = static_cast<OpenGL_RenderContext*>(context);
			return new OpenGL_Swapchain(glRenderContext, desc);
		}
		ENGINE_API void DestroySwapchain(Swapchain* swapchain) { DESTROY_OBJECT_CHECKED(swapchain); }

		ENGINE_API [[nodiscard]] RenderCommands* CreateRenderCommands(RenderContext* context, Swapchain* swapchain) {  // TODO(0x): Wrap this functions and deconstructures into allocator
			CHECK_DEPENDEND(context);
			OpenGL_RenderContext* glRenderContext = static_cast<OpenGL_RenderContext*>(context);
			OpenGL_Swapchain* glSwapchain = static_cast<OpenGL_Swapchain*>(swapchain);
			return new OpenGL_RenderCommands(glRenderContext, glSwapchain);
		}
		ENGINE_API void DestroyRenderCommands(RenderCommands* commands) { DESTROY_OBJECT_CHECKED(commands); }

		ENGINE_API [[nodiscard]] Image* CreateImage(RenderCommands* commands, const ImageDesc& desc) {
			CHECK_DEPENDEND(commands);
			return new OpenGL_Image(commands, desc);
		}

		ENGINE_API [[nodiscard]] Buffer* CreateBuffer(RenderCommands* commands, const BufferDesc& desc) {
			CHECK_DEPENDEND(commands);
			return new OpenGL_Buffer(commands, desc);
		}
	}
}