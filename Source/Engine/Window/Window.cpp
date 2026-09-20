#include <Engine/Window/Window.h>
#include <Engine/Core/Logger.h>
#include <Engine/Core/Memory.h>
#include <GLFW/glfw3.h>

namespace CusEngine {
	Window::Window(const WindowDesc& desc) : _desc(desc) {
		_handle = glfwCreateWindow(_desc.width, _desc.height, _desc.title.c_str(), nullptr, nullptr);
		Logger::Assert(_handle, "GLFW", "Failed to create window");
		Logger::Info("GLFW", "Window created width: {}, height: {}, title: {}", _desc.width, _desc.height, _desc.title);
		glfwMakeContextCurrent(_handle);

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
		contextDesc.windowHandle = _handle;

		RHI::SwapchainDesc swapchainDesc{};
		swapchainDesc.width = _desc.width;
		swapchainDesc.height = _desc.height;
		swapchainDesc.vsync = false;

		_context = RHI::CreateContext(contextDesc);
		_swapchain = _context->CreateSwapchain(swapchainDesc);

		RHI::CommandsDesc commandsDesc{};
		commandsDesc.targetSwapchain = _swapchain;
		_commands = _context->CreateCommands(commandsDesc);
	}

	Window::~Window() {
		_context->WaitDeviceIdle();

		Mem::Allocator::Destroy<RHI::Commands>(_commands);
		Mem::Allocator::Destroy<RHI::Swapchain>(_swapchain);
		Mem::Allocator::Destroy<RHI::Context>(_context);

		glfwDestroyWindow(_handle);
	}

	bool Window::ShouldClose() const { return glfwWindowShouldClose(_handle); }
	void Window::PollEvents() const { glfwPollEvents(); }
}