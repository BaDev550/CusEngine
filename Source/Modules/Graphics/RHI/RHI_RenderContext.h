#pragma once
#include "Core/Core.h"

#include "RHI.h"

namespace Graphics {
	class Texture2D;
	class ENGINE_API RHI_RenderContext {
	public:
		RHI_RenderContext() = default;
		virtual ~RHI_RenderContext() = default;

		virtual void TransitionImageLayout(RHI_CommandBufferHandle cmd, RHI_Image* image, RHI_ImageLayout newLayout) = 0;
		virtual void WaitDeviceIdle() = 0;

		virtual void UpdateTextureDescriptors(const std::vector<Memory::Ref<Texture2D>>& textures) = 0; // TEMP
		virtual u32 AddSampler(uptr sampler) = 0;

		virtual RHI_ContextHandle GetNativeHandle() const = 0;
		virtual RHI_DeviceHandle GetDeviceHandle() const = 0;
		virtual RHI_GPUFeatures* GetGPUFeatures() = 0;
		virtual void Shutdown() = 0;
	};
}