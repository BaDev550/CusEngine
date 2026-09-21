#include "RenderSubsystem.h"
#include <Engine/Core/Engine.h>
#include <Engine/Window/WindowSubsystem.h>
#include <imgui.h>

namespace CusEngine {
	bool RenderSubsystem::OnCreate(Engine* engine) {
		Subsystem::OnCreate(engine);

		auto window = engine->GetSubsystem<WindowSubsystem>()->GetWindow();

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
		contextDesc.windowHandle = window->GetHandle();

		RHI::SwapchainDesc swapchainDesc{};
		swapchainDesc.width = window->GetWidth();
		swapchainDesc.height = window->GetHeight();
		swapchainDesc.vsync = false;

		_context = RHI::CreateContext(contextDesc);
		_context->InitializeImGui();

		_swapchain = _context->CreateSwapchain(swapchainDesc);

		RHI::CommandsDesc commandsDesc{};
		commandsDesc.targetSwapchain = _swapchain;
		_commands = _context->CreateCommands(commandsDesc);
		return true;
	}

	void RenderSubsystem::OnUpdate() {
		_commands->BeginFrame();

		uint32_t imageIndex = _commands->GetImageIndex();

		_commands->BeginImGui();

		ImGui::ShowDemoWindow();

		RHI::Image* colorAttachment = _swapchain->GetColorAttachments()[imageIndex];
		RHI::Image* depthAttachment = _swapchain->GetDepthAttachment();
		_commands->BeginDynamicRendering({ colorAttachment }, depthAttachment, _swapchain->GetExtent());

		_commands->EndImGui();

		_commands->EndDynamicRendering();
		_commands->TransitionImageLayout(colorAttachment, RHI::ImageLayout::PresentSrc);

		_commands->EndFrame();
	}

	void RenderSubsystem::OnDestroy() {
		Subsystem::OnDestroy();

		_context->WaitDeviceIdle();

		Mem::Allocator::Destroy<RHI::Commands>(_commands);
		Mem::Allocator::Destroy<RHI::Swapchain>(_swapchain);
		_context->DestroyImGui();
		Mem::Allocator::Destroy<RHI::Context>(_context);
	}

	void RenderSubsystem::GetDependencyGraph(DependencyGraph & graph) {
		graph.Require<WindowSubsystem>(DependencyOrder::After);
	}
}