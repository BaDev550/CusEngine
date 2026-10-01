#pragma once
#include <Engine/Core/Core.h>

#include <Runtime/RHI/Context/RHIContextDesc.h>

namespace Runtime::RHI {
	class Image;
	class Buffer;
	class Swapchain;
	class CommandBuffer;
	class CommandPool;
	class Queue;
	class Fence;

	struct ImageDesc;
	struct BufferDesc;
	struct SwapchainDesc;
	struct CommandBufferDesc;
	struct CommandPoolDesc;
	struct QueueDesc;
	struct FenceDesc;

	class Context {
	public:
		Context() = default;
		virtual ~Context() = default;

		virtual void InitializeImGui() = 0;
		virtual void NewFrameImGui() = 0;
		virtual void DestroyImGui() = 0;

		virtual void WaitDeviceIdle() = 0;
		virtual void Shutdown() = 0;

		virtual Buffer* CreateBuffer(const BufferDesc& desc) = 0;
		virtual Image* CreateImage(const ImageDesc& desc) = 0;
		virtual Swapchain* CreateSwapchain(const SwapchainDesc& desc) = 0;
		virtual CommandPool* CreateCommandPool(const CommandPoolDesc& desc) = 0;
		virtual Queue* CreateQueue(const QueueDesc& desc) = 0;
		virtual Fence* CreateFence(const FenceDesc& desc) = 0;

		virtual ContextDesc* GetDesc() = 0;
	};

	Context* CreateContext(const ContextDesc& desc);
}