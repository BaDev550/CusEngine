#include "RenderSubsystem.h"
#include <Engine/Core/Engine.h>
#include <Engine/Window/WindowSubsystem.h>
#include <Engine/Asset/AssetSubsystem.h>

#include <Runtime/Definitions/Profiler.h>
#include <Runtime/RHI/Command/RHICommandBufferDesc.h>
#include <Runtime/RHI/Command/RHICommandPoolDesc.h>
#include <Runtime/RHI/Sync/RHIFenceDesc.h>
#include <Runtime/RHI/Queue/RHIQueue.h>

#include <imgui.h>

namespace CusEngine {
	Runtime::Result RenderSubsystem::OnCreate(Engine* engine) {
		Subsystem::OnCreate(engine);

		auto window = engine->GetSubsystem<WindowSubsystem>()->GetWindow();
		if (!window) { return Runtime::Result("Failed to find window"); }

		Runtime::RHI::ContextDesc contextDesc{};
#ifdef _DEBUG
		contextDesc.enableValidationLayer = true;
#endif
		contextDesc.features.dynamicRendering = true;
		contextDesc.features.bufferDeviceAddress = true;
		contextDesc.features.descriptorIndexing = true;
		contextDesc.features.robustBufferAccess = true;
		contextDesc.features.runtimeDescriptorArray = true;
		contextDesc.features.synchronization2 = true;
		contextDesc.features.timelineSemaphore = true;
		contextDesc.features.samplerAnisotropy = true;
		contextDesc.windowHandle = window->GetHandle();

		Runtime::RHI::SwapchainDesc swapchainDesc{};
		swapchainDesc.width = window->GetWidth();
		swapchainDesc.height = window->GetHeight();
		swapchainDesc.vsync = false;

		BEGIN_SCOPE(RHIInitilization)
		_context = Runtime::RHI::CreateContext(contextDesc);
		if (!_context) return Runtime::Result("Failed to create context");
		_context->InitializeImGui();

		_swapchain = _context->CreateSwapchain(swapchainDesc);

		{
			for (auto& fd : _frames) {
				Runtime::RHI::CommandPoolDesc cmdPoolDesc{};
				cmdPoolDesc.queueFamilyIndex = _context->GetGraphicsQueue()->GetQueueFamilyIndex();
				cmdPoolDesc.usage = Runtime::RHI::CommandPoolUsage::Resettable;
				fd.commandPool = _context->CreateCommandPool(cmdPoolDesc);

				Runtime::RHI::CommandBufferDesc cmdBufferDesc{};
				cmdBufferDesc.type = Runtime::RHI::CommandBufferType::Primary;
				fd.commandBuffer = fd.commandPool->AllocateCommandBuffer(cmdBufferDesc);

				Runtime::RHI::FenceDesc fenceDesc{};
				fenceDesc.type = Runtime::RHI::SemaphoreType::Binary;
				fd.imageAvailableFence = _context->CreateFence(fenceDesc);
			}
		}

		{
			Runtime::RHI::FenceDesc fenceDesc{};
			fenceDesc.initialValue = MaxFramesInFlight;
			fenceDesc.type = Runtime::RHI::SemaphoreType::Timeline;
			_timelineFence = _context->CreateFence(fenceDesc);
		}

		{
			_renderFinishedFences.reserve(_swapchain->GetImageCount());
			for (usize i = 0; i < _swapchain->GetImageCount(); i++) {
				Runtime::RHI::FenceDesc fenceDesc{};
				fenceDesc.type = Runtime::RHI::SemaphoreType::Binary;
				_renderFinishedFences.push_back(_context->CreateFence(fenceDesc));
			}
		}

		END_SCOPE(RHIInitilization)
		
		return Runtime::Result();
	}

	void RenderSubsystem::OnUpdate() { }

	void RenderSubsystem::OnDestroy() {
		Subsystem::OnDestroy();

		_context->WaitDeviceIdle();

		for (auto& fence : _renderFinishedFences) { Runtime::Mem::Allocator::Destroy<Runtime::RHI::Fence>(fence); }
		for (auto& frame : _frames) {
			Runtime::Mem::Allocator::Destroy<Runtime::RHI::CommandBuffer>(frame.commandBuffer);
			Runtime::Mem::Allocator::Destroy<Runtime::RHI::CommandPool>(frame.commandPool);
			Runtime::Mem::Allocator::Destroy<Runtime::RHI::Fence>(frame.imageAvailableFence);
		}

		Runtime::Mem::Allocator::Destroy<Runtime::RHI::Fence>(_timelineFence);
		Runtime::Mem::Allocator::Destroy<Runtime::RHI::Swapchain>(_swapchain);
		_context->DestroyImGui();
		_context->Shutdown();
		Runtime::Mem::Allocator::Destroy<Runtime::RHI::Context>(_context);
	}

	void RenderSubsystem::GetDependencyGraph(DependencyGraph & graph) {
		graph.Require<AssetSubsystem>(DependencyOrder::Before);
		graph.Require<WindowSubsystem>(DependencyOrder::After);
	}

	void RenderSubsystem::BeginFrame() {
		if (_recreateSwapchainNextFrame) {
			auto* window = _engine->GetSubsystem<WindowSubsystem>()->GetWindow();
			_context->WaitDeviceIdle();
			_swapchain->Destroy();
			_swapchain->Recreate(window->GetWidth(), window->GetHeight());
			_recreateSwapchainNextFrame = false;
		}

		_currentFrameIndex = _signalValue % MaxFramesInFlight;
		const u64 waitValue = _nextSignalValue - MaxFramesInFlight;
		FrameData& frame = _frames[_currentFrameIndex];

		_timelineFence->Wait(waitValue);

		Runtime::Result result = _swapchain->AcquireNextImage(_imageIndex, frame.imageAvailableFence);
		if (result.GetMessage() == "SwapchainIsOutOfDate") {
			_recreateSwapchainNextFrame = true;
			BeginFrame();
			return;
		}
		else if (result.GetMessage() == "FailedToAcquireSwapchainImage") {
			_recreateSwapchainNextFrame = true;
		}

		for (auto& fo : frame.trackedObjects) { Runtime::Mem::Allocator::Destroy(fo); }
		frame.trackedObjects.clear();

		frame.commandPool->Reset();
		frame.commandBuffer->Begin();
		_frameRecording = true;

		for (auto& command : _commandQueue) { command(frame.commandBuffer); }
		_commandQueue.clear();
	}

	void RenderSubsystem::EndFrame() {
		if (!_frameRecording) return;

		FrameData* frame = GetCurrentFrameData();
		Runtime::RHI::Queue* graphicsQueue = _context->GetGraphicsQueue();

		frame->commandBuffer->End();

		_timelineFence->Signal(_nextSignalValue++);
		graphicsQueue->Submit(
			frame->commandBuffer, 
			{ frame->imageAvailableFence }, 
			{ _renderFinishedFences[_imageIndex], _timelineFence }
		);

		_signalValue++;

		graphicsQueue->Present(_swapchain, _imageIndex, { _renderFinishedFences[_imageIndex] });
	}

	void RenderSubsystem::Submit(CommandFunc func) { _commandQueue.push_back(func); }
	void RenderSubsystem::Track(Runtime::RHI::Object* object) { GetCurrentFrameData()->trackedObjects.push_back(object); }

	void RenderSubsystem::BeginSwapchainPass() {
		FrameData* fd = GetCurrentFrameData();

		Runtime::RHI::Image* colorAttachmentImage = _swapchain->GetColorAttachments()[_imageIndex];
		Runtime::RHI::Image* depthAttachmentImage = _swapchain->GetDepthAttachment();

		Runtime::RHI::ColorAttachment colorAttachment{};
		colorAttachment.image = colorAttachmentImage;
		colorAttachment.clearColor = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);

		Runtime::RHI::RenderingSubmitInfo info{};
		info.colorAttachments = { colorAttachment };
		info.depthAttachment = depthAttachmentImage;
		info.extent = _swapchain->GetExtent();

		fd->commandBuffer->BeginDynamicRendering(info);
	}

	void RenderSubsystem::EndSwapchainPass()
	{
		FrameData* fd = GetCurrentFrameData();

		Runtime::RHI::Image* colorAttachmentImage = _swapchain->GetColorAttachments()[_imageIndex];

		fd->commandBuffer->EndDynamicRendering();
		fd->commandBuffer->TransitionImageLayout(colorAttachmentImage, Runtime::RHI::ImageLayout::PresentSrc);
	}

	RenderSubsystem::FrameData* RenderSubsystem::GetCurrentFrameData() {
		Logger::Assert(_frameRecording, "RenderSubsystem", "No active frame");
		return &_frames[_currentFrameIndex];
	}
}