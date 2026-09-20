#pragma once

#include "Graphics/RHI/RHI_RenderContext.h"
#include "Graphics/RHI/RHI_Utils.h"

#include <glad/glad.h>
#include <vector>
#include <mutex>

namespace CusEngine::RHI {
	class OpenGL_RenderContext final : public RenderContext {
	public:
		OpenGL_RenderContext(const RenderContextDesc& desc);
		virtual ~OpenGL_RenderContext();

		virtual void Shutdown() final override;
		virtual void WaitDeviceIdle() override;
		virtual GPUFeatures* GetGPUFeatures() override final { return &_desc.features; };
	private:
		RHI::RenderContextDesc _desc;
	};
}