#pragma once

#include "Graphics/RHI/RHI.h"
#include "Graphics/RHI/RHI_Swapchain.h"
#include "Graphics/RHI/RHI_RenderCommands.h"
#include "Graphics/RHI/RHI_RenderContext.h"
#include "Graphics/RHI/RHI_Image.h"
#include "Graphics/RHI/RHI_Buffer.h"
#include "Graphics/RHI/RHI_Pipeline.h"

#include "Graphics/Texture.h"
#include "Graphics/Framebuffer.h"

namespace Graphics {
	class IRenderer {
	public:
		virtual ~IRenderer() = default;

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;

		virtual void BeginSwapchainPass() = 0;
		virtual void EndSwapchainPass() = 0;

		virtual [[nodiscard]] RHI_RenderCommands* GetRenderCommands() const = 0;
	};
}