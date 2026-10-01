#include "RenderSubsystem.h"
#include <Engine/Core/Engine.h>
#include <Engine/Window/WindowSubsystem.h>
#include <Runtime/Definitions/Profiler.h>
#include <imgui.h>

#include <Engine/Asset/AssetSubsystem.h>

#include <Engine/Asset/Texture/Texture2D.h>
#include <Engine/Asset/Shader/Shader.h>

namespace CusEngine {
	Runtime::Result RenderSubsystem::OnCreate(Engine* engine) {
		Subsystem::OnCreate(engine);

		auto window = engine->GetSubsystem<WindowSubsystem>()->GetWindow();
		if (!window) {
			return Runtime::Result("Failed to find window");
		}

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

		END_SCOPE(RHIInitilization)

		return Runtime::Result();
	}

	void RenderSubsystem::OnUpdate() {

	}

	void RenderSubsystem::OnDestroy() {
		Subsystem::OnDestroy();

		_context->WaitDeviceIdle();

		Runtime::Mem::Allocator::Destroy<Runtime::RHI::Swapchain>(_swapchain);
		_context->DestroyImGui();
		Runtime::Mem::Allocator::Destroy<Runtime::RHI::Context>(_context);
	}

	void RenderSubsystem::GetDependencyGraph(DependencyGraph & graph) {
		graph.Require<WindowSubsystem>(DependencyOrder::After);
	}
}