#include <Engine/Window/WindowSubsystem.h>
#include <Engine/Core/Engine.h>

#include <GLFW/glfw3.h>

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
	}

	void WindowSubsystem::OnDestroy() {
		_window = nullptr;

		if (_glfwInitialized) {
			glfwTerminate();
		}
	}
}