#include "Window/Window.h"
#include "Core/Logger.h"
#include "Core/Engine.h"
#include "Graphics/RHI/RHISubsystem.h"
#include <GLFW/glfw3.h>

namespace CusEngine {
	Window::Window(Engine* engine, const WindowDesc& desc) : _desc(desc) {
		_handle = glfwCreateWindow(_desc.width, _desc.height, _desc.title.c_str(), nullptr, nullptr);
		Logger::Assert(_handle, "GLFW", "Failed to create window");
		Logger::Info("GLFW", "Window created width: {}, height: {}, title: {}", _desc.width, _desc.height, _desc.title);
		glfwMakeContextCurrent(_handle);

		auto rhi = engine->GetSubsystem<RHI::RHISubsystem>();

		RHI::GPUFeatures features{};
		features.dynamicRendering = true;
		features.bufferDeviceAddress = true;
		features.descriptorIndexing = true;
		features.robustBufferAccess = true;
		features.runtimeDescriptorArray = true;
		features.samplerAnisotropy = true;
		features.synchronization2 = true;
		features.timelineSemaphore = true;

		RHI::RenderContextDesc renderContextDesc{};
		renderContextDesc.windowHandle = _handle;
		renderContextDesc.features = features;

		_renderContext = rhi->CreateRenderContext(renderContextDesc);

		RHI::SwapchainDesc swapchainDesc{};
		swapchainDesc.width = _desc.width;
		swapchainDesc.height = _desc.height;
		swapchainDesc.vsync = false;

		_swapchain = rhi->CreateSwapchain(_renderContext, swapchainDesc);
	}

	Window::~Window() {
		glfwDestroyWindow(_handle);
	}

	bool Window::ShouldClose() const { return glfwWindowShouldClose(_handle); }
	void Window::PollEvents() const { glfwPollEvents(); }
}