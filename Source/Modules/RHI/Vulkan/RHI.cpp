#include "Graphics/RHI/RHI_RenderContext.h"
#include "Graphics/RHI/RHI_Image.h"

#include "VulkanRenderContext.h"
#include "VulkanRenderCommands.h"
#include "VulkanSwapchain.h"
#include "VulkanImage.h"
#include "VulkanBuffer.h"

#include <vector>

extern "C" {
#define CHECK_DEPENDEND(depended) Logger::Assert(depended, "RHI", "Invalid context")
#define DESTROY_OBJECT_CHECKED(obj) if (obj) delete obj
	namespace Graphics {
		ENGINE_API [[nodiscard]] RHI_RenderContext* RHI_CreateRenderContext(const RHI_RenderContextDesc& desc) { 
			return new Vulkan_RenderContext(desc); 
		}
		ENGINE_API [[nodiscard]] void RHI_DestroyRenderContext(RHI_RenderContext* context) {
			if (context) {
				context->Shutdown();
				delete context;
			}
		}

		ENGINE_API [[nodiscard]] RHI_Swapchain* RHI_CreateSwapchain(RHI_RenderContext* context, const RHI_SwapchainDesc& desc) {
			CHECK_DEPENDEND(context);
			Vulkan_RenderContext* vkRenderContext = static_cast<Vulkan_RenderContext*>(context);
			return new Vulkan_Swapchain(vkRenderContext, desc);
		}
		ENGINE_API void RHI_DestroySwapchain(RHI_Swapchain* swapchain) { DESTROY_OBJECT_CHECKED(swapchain); }

		ENGINE_API [[nodiscard]] RHI_RenderCommands* RHI_CreateRenderCommands(RHI_RenderContext* context, RHI_Swapchain* swapchain) { 
			CHECK_DEPENDEND(context);
			Vulkan_RenderContext* vkRenderContext = static_cast<Vulkan_RenderContext*>(context);
			Vulkan_Swapchain* vkSwapchain = static_cast<Vulkan_Swapchain*>(swapchain);
			return new Vulkan_RenderCommands(vkRenderContext, vkSwapchain);
		}
		ENGINE_API void RHI_DestroyRenderCommands(RHI_RenderCommands* commands) { DESTROY_OBJECT_CHECKED(commands); }

		ENGINE_API [[nodiscard]] RHI_Image* RHI_CreateImage(RHI_RenderCommands* commands, const RHI_ImageDesc& desc) {
			CHECK_DEPENDEND(commands);
			return new Vulkan_Image(commands, desc);
		}

		ENGINE_API [[nodiscard]] RHI_Buffer* RHI_CreateBuffer(RHI_RenderCommands* commands, const RHI_BufferDesc& desc) {
			CHECK_DEPENDEND(commands);
			return new Vulkan_Buffer(commands, desc);
		}
	}
}