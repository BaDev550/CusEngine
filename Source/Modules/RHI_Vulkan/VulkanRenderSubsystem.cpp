#include "VulkanRenderSubsystem.h"

#include "VulkanRenderContext.h"
#include "VulkanRenderCommands.h"
#include "VulkanSwapchain.h"
#include "VulkanImage.h"
#include "VulkanBuffer.h"

namespace CusEngine {
    using namespace RHI;

    RenderContext* Vulkan_RHISubsystem::CreateRenderContext(const RenderContextDesc& desc) 
    {
        return new Vulkan_RenderContext(desc);
    }

    void Vulkan_RHISubsystem::DestroyRenderContext(RenderContext* context) {
        if (context) {
            context->Shutdown();
            delete context;
        }
    }

    RenderCommands* Vulkan_RHISubsystem::CreateRenderCommands(RenderContext* context, Swapchain* swapchain)
    {
        CHECK_DEPENDEND(context);
        Vulkan_RenderContext* vkRenderContext = static_cast<Vulkan_RenderContext*>(context);
        Vulkan_Swapchain* vkSwapchain = static_cast<Vulkan_Swapchain*>(swapchain);
        return new Vulkan_RenderCommands(vkRenderContext, vkSwapchain);
    }

    void Vulkan_RHISubsystem::DestroyRenderCommands(RenderCommands* commands) {
        DESTROY_OBJECT_CHECKED(commands);
    }

    Swapchain* Vulkan_RHISubsystem::CreateSwapchain(RenderContext* context, const SwapchainDesc& desc) {
        CHECK_DEPENDEND(context);
        Vulkan_RenderContext* vkRenderContext = static_cast<Vulkan_RenderContext*>(context);
        return new Vulkan_Swapchain(vkRenderContext, desc);
    }

    void Vulkan_RHISubsystem::DestroySwapchain(Swapchain* swapchain) {
        DESTROY_OBJECT_CHECKED(swapchain);
    }

    Image* Vulkan_RHISubsystem::CreateImage(RenderCommands* commands, const ImageDesc& desc) {
        CHECK_DEPENDEND(commands);
        return new Vulkan_Image(commands, desc);
    }

    Buffer* Vulkan_RHISubsystem::CreateBuffer(RenderCommands* commands, const BufferDesc& desc) {
        CHECK_DEPENDEND(commands);
        return new Vulkan_Buffer(commands, desc);
    }

    Pipeline* Vulkan_RHISubsystem::CreatePipeline(RenderCommands* commands, const PipelineDesc& desc) {
        return nullptr;
    }
}