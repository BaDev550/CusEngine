#include "RenderSubsystem.h"
#include <Engine/Core/Engine.h>
#include <Engine/Reflection/ReflectionSystem.h>
#include <Engine/Window/WindowSubsystem.h>
#include <Engine/Asset/AssetSubsystem.h>

#include <Runtime/Definitions/Profiler.h>
#include <Runtime/RHI/Command/RHICommandBufferDesc.h>
#include <Runtime/RHI/Command/RHICommandPoolDesc.h>
#include <Runtime/RHI/Sync/RHIFenceDesc.h>
#include <Runtime/RHI/Queue/RHIQueue.h>

#include <Engine/Renderer/RenderGraph/RenderGraph.h>

#include <queue>

namespace Tourqe::Engine {
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
		contextDesc.instance = window->GetInstance();

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

	void RenderSubsystem::OnUpdate() {
		RHI::CommandBuffer* cmd = BeginFrame();
		if (!cmd) return;

		RenderGraph graph;

		u32 width = (u32)GetSwapchain()->GetExtent().x;
		u32 height = (u32)GetSwapchain()->GetExtent().y;

		auto backbuffer = graph.ImportImage(
			"BackBuffer",
			GetSwapchain()->GetColorAttachments()[_imageIndex],
			Runtime::RHI::ImageLayout::Undefined,
			Runtime::RHI::ImageLayout::PresentSrc
		);

		Runtime::RHI::ImageDesc depthDesc{};
		depthDesc.width = width;
		depthDesc.height = height;
		depthDesc.usage = Runtime::RHI::ImageUsage::DepthStencilAttachment;
		depthDesc.format = Runtime::RHI::Format::D32_SFLOAT;
		auto depthbuffer = graph.CreateImage("Depth", depthDesc);

		graph.AddPass("Scene",
			[&](RenderPassBuilder& builder) {
				builder.Write(backbuffer, Runtime::RHI::ImageLayout::ColorAttachment);
				builder.Write(depthbuffer, Runtime::RHI::ImageLayout::DepthAttachment);
			},
			[=](Runtime::RHI::CommandBuffer* cmd, const RenderGraphResourceContainer& res) {
				auto* color = res.GetImage(backbuffer);
				auto* depth = res.GetImage(depthbuffer);

				Runtime::RHI::ColorAttachment colorAttachment{};
				colorAttachment.image = color;
				colorAttachment.clearColor = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);

				Runtime::RHI::RenderingSubmitInfo info{};
				info.colorAttachments = { colorAttachment };
				info.depthAttachment = depth;
				info.extent = glm::vec2(width, height);

				Runtime::RHI::PassDebugInfo debugInfo{};
				debugInfo.label = "ScenePass";
				debugInfo.color = glm::vec3(1.0f, 1.0f, 0.0f);

				cmd->BeginDynamicRendering(info, &debugInfo);

				cmd->EndDynamicRendering();
			});
		graph.Compile();
		graph.Execute(cmd);

		EndFrame();
	}

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

	RHI::CommandBuffer* RenderSubsystem::BeginFrame() {
		auto* window = _engine->GetSubsystem<WindowSubsystem>()->GetWindow();
		if (window->IsMinimized()) {
			_canSubmitCommand = false;
			_commandQueue.clear();
			return nullptr;
		}

		if (_recreateSwapchainNextFrame) {
			_context->WaitDeviceIdle();
			_swapchain->Destroy();
			_swapchain->Recreate(window->GetWidth(), window->GetHeight());
			_recreateSwapchainNextFrame = false;
		}

		_currentFrameIndex = _signalValue % MaxFramesInFlight;
		const u64 waitValue = _nextSignalValue - MaxFramesInFlight;
		FrameData& frame = _frames[_currentFrameIndex];

		_timelineFence->Wait(_nextSignalValue - MaxFramesInFlight);
	
		Result result = _swapchain->AcquireNextImage(_imageIndex, frame.imageAvailableFence);
		if (result.GetMessage() == "SwapchainIsOutOfDate") {
			_recreateSwapchainNextFrame = true;
			return BeginFrame();
		}
		else if (result.GetMessage() == "FailedToAcquireSwapchainImage") {
			_recreateSwapchainNextFrame = true;
		}

		for (auto& fo : frame.trackedObjects) { Mem::Allocator::Destroy(fo); }
		frame.trackedObjects.clear();

		frame.commandPool->Reset();
		frame.commandBuffer->Begin();
		_frameRecording = true;
		_canSubmitCommand = true;

		for (auto& command : _commandQueue) { command(frame.commandBuffer); }
		_commandQueue.clear();

		return frame.commandBuffer;
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
		_frameRecording = false;
	}

	void RenderSubsystem::Submit(CommandFunc func) { 
		if (!_canSubmitCommand) return;
		_commandQueue.push_back(func); 
	}

	void RenderSubsystem::Track(RHI::Object* object) { GetCurrentFrameData()->trackedObjects.push_back(object); }

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