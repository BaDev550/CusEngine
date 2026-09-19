#include "Engine.h"
#include "Core/Logger.h"

namespace CusEngine {
	Engine::Engine() {
		WindowDesc desc{};
		desc.width = 800;
		desc.height = 800;
		desc.title = "CusEngine Renderer";
		_window = Mem::Allocator::ConstructUnique<Window>(desc);
	}

	Engine::~Engine() {
		Logger::Info("Engine", "Shuting down...");

	}

	void Engine::Run() {
		Logger::Info("Engine", "Engine running...");

		while (!_window->ShouldClose() && _running) {

			_window->PollEvents();
		}
	}

	void Engine::Shutdown(const std::string_view reson) {
		Logger::Info("Engine", "Closing reson: {}", reson.data());
		_running = false;
	}
}