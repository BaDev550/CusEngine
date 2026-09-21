#include <Engine/Window/WindowSubsystem.h>
#include <Engine/Core/Engine.h>

#include <GLFW/glfw3.h>
#include <imgui.h>

namespace CusEngine {
	bool WindowSubsystem::OnCreate(Engine* engine)
	{
		Subsystem::OnCreate(engine);

		if (!_glfwInitialized) {
			Logger::Assert(glfwInit(), "GLFW", "Failed to initialize GLFW context");
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
			_glfwInitialized = true;
		}

		WindowDesc desc{};
		desc.width = 800;
		desc.height = 800;
		desc.title = "Engine Debug Window";
		_window = Mem::Allocator::ConstructUnique<Window>(desc);

		return true;
	}

	void WindowSubsystem::OnUpdate() {
		_window->PollEvents();

		if (_window->ShouldClose()) {
			_engine->Shutdown("Window closed");
		}

		_window->_commands->BeginFrame();

		uint32_t imageIndex = _window->_commands->GetImageIndex();

		_window->_commands->BeginImGui();

		ImGui::ShowDemoWindow();

		RHI::Image* colorAttachment = _window->_swapchain->GetColorAttachments()[imageIndex];
		RHI::Image* depthAttachment = _window->_swapchain->GetDepthAttachment();
		_window->_commands->BeginDynamicRendering({ colorAttachment }, depthAttachment, _window->_swapchain->GetExtent());

		_window->_commands->EndImGui();

		_window->_commands->EndDynamicRendering();
		_window->_commands->TransitionImageLayout(colorAttachment, RHI::ImageLayout::PresentSrc);

		_window->_commands->EndFrame();
	}

	void WindowSubsystem::OnDestroy() {
		_window = nullptr;

		if (_glfwInitialized) {
			glfwTerminate();
		}
	}
}