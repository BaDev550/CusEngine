#include "Core/Window.h"
#include "Core/Logger.h"
#include <GLFW/glfw3.h>

namespace CusEngine {
	static bool s_glfwInitialized = false;

	Window::Window(const WindowDesc& desc) : _desc(desc) {
		if (!s_glfwInitialized) {
			Logger::Assert(glfwInit(), "GLFW", "Failed to initialize GLFW context");
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			s_glfwInitialized = true;
		}
		_handle = glfwCreateWindow(_desc.width, _desc.height, _desc.title.c_str(), nullptr, nullptr);
		Logger::Assert(_handle, "GLFW", "Failed to create window");
		Logger::Info("GLFW", "Window created width: {}, height: {}, title: {}", _desc.width, _desc.height, _desc.title);
		glfwMakeContextCurrent(_handle);
	}

	Window::~Window() {
		glfwDestroyWindow(_handle);
		if (s_glfwInitialized) {
			glfwTerminate();
		}
	}

	bool Window::ShouldClose() const { return glfwWindowShouldClose(_handle); }
	void Window::PollEvents() const { glfwPollEvents(); }
}