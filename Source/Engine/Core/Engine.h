#pragma once
#include "Core/Window.h"
#include "Core/Memory.h"
#include <string_view>

namespace CusEngine {
	class Engine final {
	public:
		Engine();
		~Engine();
		Engine(const Engine&) = delete;
		Engine& operator=(const Engine&) = delete;

		void Run();
		void Shutdown(const std::string_view reson);
	private:
		bool _running = true;

		Mem::Unique<Window> _window;
	};
}