#pragma once
#include <Engine/Core/Core.h>

#include <Runtime/RHI/Context/RHIContextDesc.h>

namespace CusEngine::RHI {
	class Commands;
	class Image;
	class Buffer;
	class Swapchain;

	struct ImageDesc;
	struct BufferDesc;
	struct SwapchainDesc;
	struct CommandsDesc;

	class ENGINE_API Context {
	public:
		Context() = default;
		virtual ~Context() = default;

		virtual void InitializeImGui() = 0;
		virtual void NewFrameImGui() = 0;
		virtual void DestroyImGui() = 0;

		virtual void WaitDeviceIdle() = 0;
		virtual void Shutdown() = 0;

		virtual Commands* CreateCommands(const CommandsDesc& desc) = 0;
		virtual Buffer* CreateBuffer(const BufferDesc& desc) = 0;
		virtual Image* CreateImage(const ImageDesc& desc) = 0;
		virtual Swapchain* CreateSwapchain(const SwapchainDesc& desc) = 0;

		virtual ContextDesc* GetDesc() = 0;
	};

	Context* CreateContext(const ContextDesc& desc);
}