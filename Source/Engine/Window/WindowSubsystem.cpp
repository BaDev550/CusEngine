#include "Window/WindowSubsystem.h"
#include "Core/Engine.h"

#include <GLFW/glfw3.h>

namespace CusEngine {
	bool WindowSubsystem::OnCreate(Engine* engine)
	{
		Subsystem::OnCreate(engine);

		if (!_glfwInitialized) {
			Logger::Assert(glfwInit(), "GLFW", "Failed to initialize GLFW context");
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			_glfwInitialized = true;
		}

		WindowDesc desc{};
		desc.width = 800;
		desc.height = 800;
		desc.title = "Engine Debug Window";
		Mem::Unique<Window> tempWindow = Mem::Allocator::ConstructUnique<Window>(desc);

		_windowList.push_back(std::move(tempWindow));
		return true;
	}

	void WindowSubsystem::OnUpdate() {
		for (auto& window : _windowList) {
			window->PollEvents();

			if (window->ShouldClose()) {
				_engine->Shutdown("Window closed");
			}
		}
	}

	void WindowSubsystem::OnDestroy() {
		_windowList.clear();
		if (_glfwInitialized) {
			glfwTerminate();
		}
	}
}