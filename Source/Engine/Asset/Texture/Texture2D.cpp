#include "Texture2D.h"

#include <Engine/Renderer/RenderSubsystem.h>
#include <Runtime/RHI/Buffer/RHIBuffer.h>
#include <Runtime/RHI/Image/RHIImage.h>
#include <Runtime/RHI/Context/RHIContext.h>

namespace CusEngine {
    Texture2D::Texture2D(int width, int height) {
        auto* renderSubsystem = Engine::Get()->GetSubsystem<RenderSubsystem>();
        auto* rhi_context = renderSubsystem->GetContext();
        auto* rhi_commands = renderSubsystem->GetCommands();

        RHI::ImageDesc desc{};
        desc.format = RHI::Format::BC3; // TEMP
        desc.width = width;
        desc.height = height;
        desc.layout = RHI::ImageLayout::TransferDst;
        desc.usage = RHI::ImageUsage::Sampled | RHI::ImageUsage::TransferDst;
        desc.tileMode = RHI::ImageTileMode::Optimal;
        desc.view.type = RHI::ImageViewType::Image2D;
        _image = rhi_context->CreateImage(desc);
    }
}