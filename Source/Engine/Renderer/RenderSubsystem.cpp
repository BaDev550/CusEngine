#include "RenderSubsystem.h"
#include <Engine/Core/Engine.h>
#include <Engine/Window/WindowSubsystem.h>
#include <Engine/Core/Profiler.h>
#include <imgui.h>

#include <Engine/Asset/AssetSubsystem.h>
#include <Engine/Asset/Texture/Texture2D.h>

namespace CusEngine {
	Result RenderSubsystem::OnCreate(Engine* engine) {
		Subsystem::OnCreate(engine);

		auto window = engine->GetSubsystem<WindowSubsystem>()->GetWindow();
		if (!window) {
			return Result("Failed to find window");
		}

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
		if (!_context) return Result("Failed to create context");
		_context->InitializeImGui();

		_swapchain = _context->CreateSwapchain(swapchainDesc);

		RHI::CommandsDesc commandsDesc{};
		commandsDesc.targetSwapchain = _swapchain;
		_commands = _context->CreateCommands(commandsDesc);
		if (!_commands) return Result("Failed to create commands");
		END_SCOPE(RHIInitilization)

		{
			_imguiPass.width = 800;
			_imguiPass.height = 800;

			RHI::ImageDesc color_pass_desc{};
			color_pass_desc.width = _imguiPass.width;
			color_pass_desc.height = _imguiPass.height;
			color_pass_desc.format = RHI::Format::RGBA8;
			color_pass_desc.tileMode = RHI::ImageTileMode::Optimal;
			color_pass_desc.layout = RHI::ImageLayout::Undefined;
			color_pass_desc.usage = RHI::ImageUsage::ColorAttachment;
			color_pass_desc.view.type = RHI::ImageViewType::Image2D;
			RHI::Image* color_pass = _context->CreateImage(color_pass_desc);
			_imguiPass.colorAttachments.push_back(std::move(color_pass));
		}
		return Result();
	}

	void RenderSubsystem::OnUpdate() {
		_commands->BeginFrame();

		uint32_t imageIndex = _commands->GetImageIndex();

		RHI::Image* colorAttachment = _swapchain->GetColorAttachments()[imageIndex];
		RHI::Image* depthAttachment = _swapchain->GetDepthAttachment();

		//{ write imgui visual into buffer
		//	_commands->BeginImGui();
		//	ImGui::ShowDemoWindow();
		//	_commands->BeginDynamicRendering(_imguiPass.colorAttachments, depthAttachment, { _imguiPass.width, _imguiPass.height });
		//	_commands->EndImGui();
		//	_commands->EndDynamicRendering();
		//	_commands->TransitionImageLayout(_imguiPass.colorAttachments[0], RHI::ImageLayout::ShaderReadOnly);
		//}

		_commands->BeginImGui();
		ImGui::ShowDemoWindow();

		ImGui::Begin("Debug");

		if (ImGui::Button("Parse FBX")) {
			auto* assetSystem = Engine::Get()->GetSubsystem<AssetSubsystem>();

			Texture2D* textureAsset = assetSystem->Get<Texture2D>("guven-catak.jpg");
			if (textureAsset) {
				Logger::Info("RenderSubsystem", "Texture loaded to CPU");
			}
		}

		ImGui::End();

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