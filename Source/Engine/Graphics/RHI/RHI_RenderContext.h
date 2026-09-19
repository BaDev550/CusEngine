#pragma once
#include "Core/Core.h"

#include "RHI.h"

namespace CusEngine::RHI {
	class ENGINE_API RenderContext {
	public:
		RenderContext() = default;
		virtual ~RenderContext() = default;

		virtual void WaitDeviceIdle() = 0;
		virtual void Shutdown() = 0;

		//virtual void UpdateTextureDescriptors(const std::vector<Mem::Ref<Texture2D>>& textures) = 0; // TEMP
		//virtual u32 AddSampler(uptr sampler) = 0;

		virtual GPUFeatures* GetGPUFeatures() = 0;
	};
}