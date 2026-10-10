#include <Engine/Window/Window.h>
#include <Runtime/Definitions/Logger.h>
#include <Runtime/Memory/Memory.h>
#include <GLFW/glfw3.h>

namespace Tourqe::Engine {
	Window::Window(const WindowDesc& desc) : _desc(desc) {
		_handle = glfwCreateWindow(_desc.width, _desc.height, _desc.title.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(_handle, &_desc);
		Logger::Assert(_handle, "GLFW", "Failed to create window");
		Logger::Info("GLFW", "Window created width: {}, height: {}, title: {}", _desc.width, _desc.height, _desc.title);
		glfwMakeContextCurrent(_handle);
		glfwSetFramebufferSizeCallback(_handle, GLFWResizeEvent);
	}

	Window::~Window() {
		glfwDestroyWindow(_handle);
	}

	bool Window::ShouldClose() const { return glfwWindowShouldClose(_handle); }
	void Window::PollEvents() const { glfwPollEvents(); }

	void Window::GLFWResizeEvent(GLFWwindow* window, int width, int height) {
		WindowDesc* desc = reinterpret_cast<WindowDesc*>(glfwGetWindowUserPointer(window));
		if (desc) {
			desc->width = width;
			desc->height = height;
		}
	}

}