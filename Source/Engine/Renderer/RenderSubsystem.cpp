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
	Result RenderSubsystem::OnCreate(Engine* engine) {
		Subsystem::OnCreate(engine);

		auto window = engine->GetSubsystem<WindowSubsystem>()->GetWindow();
		if (!window) { return Result("Failed to find window"); }

		RHI::ContextDesc contextDesc{};
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

		RHI::SwapchainDesc swapchainDesc{};
		swapchainDesc.width = window->GetWidth();
		swapchainDesc.height = window->GetHeight();
		swapchainDesc.vsync = false;

		BEGIN_SCOPE(RHIInitilization)
		_context = RHI::CreateContext(contextDesc);
		if (!_context) 
			return Result("Failed to create context");
		_context->InitializeImGui();

		_swapchain = _context->CreateSwapchain(swapchainDesc);

		{
			for (auto& fd : _frames) {
				RHI::CommandPoolDesc cmdPoolDesc{};
				cmdPoolDesc.queueFamilyIndex = _context->GetGraphicsQueue()->GetQueueFamilyIndex();
				cmdPoolDesc.usage = RHI::CommandPoolUsage::Resettable;
				fd.commandPool = _context->CreateCommandPool(cmdPoolDesc);

				RHI::CommandBufferDesc cmdBufferDesc{};
				cmdBufferDesc.type = RHI::CommandBufferType::Primary;
				fd.commandBuffer = fd.commandPool->AllocateCommandBuffer(cmdBufferDesc);

				RHI::FenceDesc fenceDesc{};
				fenceDesc.type = RHI::SemaphoreType::Binary;
				fd.imageAvailableFence = _context->CreateFence(fenceDesc);
			}
		}

		{
			RHI::FenceDesc fenceDesc{};
			fenceDesc.initialValue = MaxFramesInFlight;
			fenceDesc.type = RHI::SemaphoreType::Timeline;
			_timelineFence = _context->CreateFence(fenceDesc);
		}

		{
			_renderFinishedFences.reserve(_swapchain->GetImageCount());
			for (usize i = 0; i < _swapchain->GetImageCount(); i++) {
				RHI::FenceDesc fenceDesc{};
				fenceDesc.type = RHI::SemaphoreType::Binary;
				_renderFinishedFences.push_back(_context->CreateFence(fenceDesc));
			}
		}

		{
			RHI::ImageDesc imageDesc{};
			imageDesc.width = 1;
			imageDesc.height = 1;
			imageDesc.format = RHI::Format::RGBA8;
			imageDesc.usage = RHI::ImageUsage::Sampled | RHI::ImageUsage::TransferDst;
			imageDesc.view.type = RHI::ImageViewType::Image2D;
			imageDesc.tileMode = RHI::ImageTileMode::Optimal;
			imageDesc.sampler = RHI::StaticSampler::NearestClamp;
			u32 whiteImageData = COLOR_WHITE;

			_defaultWhiteImage = _context->CreateImage(imageDesc);
			
			Submit([=](RHI::CommandBuffer* cmd) {
				RHI::Buffer* stagingBuffer = CreateStagingBuffer(sizeof(u32));
				stagingBuffer->Write(&whiteImageData);

				cmd->TransitionImageLayout(_defaultWhiteImage, RHI::ImageLayout::TransferDst);
				cmd->CopyBufferToImage(stagingBuffer, _defaultWhiteImage, RHI::ImageLayout::TransferDst, imageDesc.width, imageDesc.height);
				cmd->TransitionImageLayout(_defaultWhiteImage, RHI::ImageLayout::ShaderReadOnly);
				_defaultWhiteImage->GetBindlessIndex();
				});
		}

		END_SCOPE(RHIInitilization)
		
		return Result();
	}

	void RenderSubsystem::OnUpdate() { }

	void RenderSubsystem::OnDestroy() {
		Subsystem::OnDestroy();

		_context->WaitDeviceIdle();

		Mem::Allocator::Destroy<RHI::Image>(_defaultWhiteImage);

		for (auto& fence : _renderFinishedFences) { Mem::Allocator::Destroy<RHI::Fence>(fence); }
		for (auto& frame : _frames) {
			Mem::Allocator::Destroy<RHI::CommandBuffer>(frame.commandBuffer);
			Mem::Allocator::Destroy<RHI::CommandPool>(frame.commandPool);
			Mem::Allocator::Destroy<RHI::Fence>(frame.imageAvailableFence);
		}

		Mem::Allocator::Destroy<RHI::Fence>(_timelineFence);
		Mem::Allocator::Destroy<RHI::Swapchain>(_swapchain);
		_context->DestroyImGui();
		_context->Shutdown();
		Mem::Allocator::Destroy<RHI::Context>(_context);
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

		Result result = _swapchain->AcquireNextImage(_imageIndex, frame.imageAvailableFence);
		if (result.GetMessage() == "SwapchainIsOutOfDate") {
			_recreateSwapchainNextFrame = true;
			BeginFrame();
			return;
		}
		else if (result.GetMessage() == "FailedToAcquireSwapchainImage") {
			_recreateSwapchainNextFrame = true;
		}

		for (auto& fo : frame.trackedObjects) { Mem::Allocator::Destroy(fo); }
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
		RHI::Queue* graphicsQueue = _context->GetGraphicsQueue();

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

	void RenderSubsystem::Track(RHI::Object* object) { GetCurrentFrameData()->trackedObjects.push_back(object); }

	void RenderSubsystem::BeginSwapchainPass() {
		FrameData* fd = GetCurrentFrameData();

		RHI::Image* colorAttachmentImage = _swapchain->GetColorAttachments()[_imageIndex];
		RHI::Image* depthAttachmentImage = _swapchain->GetDepthAttachment();

		RHI::ColorAttachment colorAttachment{};
		colorAttachment.image = colorAttachmentImage;
		colorAttachment.clearColor = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);

		RHI::RenderingSubmitInfo info{};
		info.colorAttachments = { colorAttachment };
		info.depthAttachment = depthAttachmentImage;
		info.extent = _swapchain->GetExtent();

		fd->commandBuffer->BeginDynamicRendering(info);
	}

	void RenderSubsystem::EndSwapchainPass()
	{
		FrameData* fd = GetCurrentFrameData();

		RHI::Image* colorAttachmentImage = _swapchain->GetColorAttachments()[_imageIndex];

		fd->commandBuffer->EndDynamicRendering();
		fd->commandBuffer->TransitionImageLayout(colorAttachmentImage, RHI::ImageLayout::PresentSrc);
	}

	RHI::Buffer* RenderSubsystem::CreateStagingBuffer(usize dataSizeInBytes)
	{
		RHI::BufferDesc desc{};
		desc.usage = RHI::BufferUsage::TransferSrc;
		desc.memoryUsage = RHI::MemoryUsage::CPUToGPU;
		desc.allocationFlags = RHI::AllocationFlagBits::HostAccessSequentialWrite | RHI::AllocationFlagBits::CreateMapped;
		desc.size = dataSizeInBytes;
		RHI::Buffer* stagingBuffer = _context->CreateBuffer(desc);
		stagingBuffer->SetObjectDebugName("RHIObjectStagingBuffer");
		
		Track(stagingBuffer);
		return stagingBuffer;
	}

	RenderSubsystem::FrameData* RenderSubsystem::GetCurrentFrameData() {
		Logger::Assert(_frameRecording, "RenderSubsystem", "No active frame");
		return &_frames[_currentFrameIndex];
	}
}