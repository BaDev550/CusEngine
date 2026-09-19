#include <Graphics/RHI/RHI_RenderContext.h>
#include <Graphics/RHI/RHI_Image.h>

#include "VulkanRenderContext.h"
#include "VulkanRenderCommands.h"
#include "VulkanSwapchain.h"
#include "VulkanImage.h"
#include "VulkanBuffer.h"

#include <vector>

extern "C" {
#define CHECK_DEPENDEND(depended) Logger::Assert(depended, "RHI", "Invalid context")
#define DESTROY_OBJECT_CHECKED(obj) if (obj) delete obj

	namespace CusEngine::RHI {
		ENGINE_API [[nodiscard]] RenderContext* CreateRenderContext(const RenderContextDesc& desc) { 
			return new Vulkan_RenderContext(desc); 
		}

		ENGINE_API [[nodiscard]] void DestroyRenderContext(RenderContext* context) {
			if (context) {
				context->Shutdown();
				delete context;
			}
		}

		ENGINE_API [[nodiscard]] Swapchain* CreateSwapchain(RenderContext* context, const SwapchainDesc& desc) {
			CHECK_DEPENDEND(context);
			Vulkan_RenderContext* vkRenderContext = static_cast<Vulkan_RenderContext*>(context);
			return new Vulkan_Swapchain(vkRenderContext, desc);
		}
		ENGINE_API void DestroySwapchain(Swapchain* swapchain) { DESTROY_OBJECT_CHECKED(swapchain); }

		ENGINE_API [[nodiscard]] RenderCommands* CreateRenderCommands(RenderContext* context, Swapchain* swapchain) { 
			CHECK_DEPENDEND(context);
			Vulkan_RenderContext* vkRenderContext = static_cast<Vulkan_RenderContext*>(context);
			Vulkan_Swapchain* vkSwapchain = static_cast<Vulkan_Swapchain*>(swapchain);
			return new Vulkan_RenderCommands(vkRenderContext, vkSwapchain);
		}
		ENGINE_API void DestroyRenderCommands(RenderCommands* commands) { DESTROY_OBJECT_CHECKED(commands); }

		ENGINE_API [[nodiscard]] Image* CreateImage(RenderCommands* commands, const ImageDesc& desc) {
			CHECK_DEPENDEND(commands);
			return new Vulkan_Image(commands, desc);
		}

		ENGINE_API [[nodiscard]] Buffer* CreateBuffer(RenderCommands* commands, const BufferDesc& desc) {
			CHECK_DEPENDEND(commands);
			return new Vulkan_Buffer(commands, desc);
		}
	}
}