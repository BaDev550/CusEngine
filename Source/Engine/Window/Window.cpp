#include <Engine/Window/Window.h>
#include <Engine/Core/Logger.h>
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
		contextDesc.windowHandle = _handle;

		_context = RHI::CreateContext(contextDesc);
	}

	Window::~Window() {
		glfwDestroyWindow(_handle);
	}

	bool Window::ShouldClose() const { return glfwWindowShouldClose(_handle); }
	void Window::PollEvents() const { glfwPollEvents(); }
}