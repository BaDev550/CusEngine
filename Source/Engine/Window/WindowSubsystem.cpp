#include "Window/WindowSubsystem.h"
#include "Core/Engine.h"

#include <GLFW/glfw3.h>

namespace CusEngine {
	bool WindowSubsystem::OnCreate(Engine* engine)
	{
		Subsystem::OnCreate(engine);

		Logger::Info("WindowSubsystem", "Created!");

		if (!_glfwInitialized) {
			Logger::Assert(glfwInit(), "GLFW", "Failed to initialize GLFW context");
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
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
	}

	void WindowSubsystem::OnDestroy() {
		_window = nullptr;

		if (_glfwInitialized) {
			glfwTerminate();
		}
	}
}