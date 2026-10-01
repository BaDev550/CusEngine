#include "Texture2D.h"

#include <Engine/Renderer/RenderSubsystem.h>
#include <Runtime/RHI/Buffer/RHIBuffer.h>
#include <Runtime/RHI/Image/RHIImage.h>
#include <Runtime/RHI/Context/RHIContext.h>

namespace CusEngine {
    Texture2D::Texture2D(const Runtime::RHI::ImageDesc& desc) {
        auto* renderSubsystem = Engine::Get()->GetSubsystem<RenderSubsystem>();
        auto* rhi_context = renderSubsystem->GetContext();
        
        _image = rhi_context->CreateImage(desc);
    }

    Texture2D::~Texture2D() {
        Runtime::Mem::Allocator::Destroy(_image);
    }
}