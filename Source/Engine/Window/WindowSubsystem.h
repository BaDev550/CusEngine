#pragma once
#include "Core/Types.h"
#include "Core/Memory.h"
#include "Window/Window.h"
#include "Subsystem/Subsystem.h"
#include <vector>

namespace CusEngine {
	class WindowSubsystem final : public Subsystem {
	public:
		virtual bool OnCreate(Engine* engine) override;
		virtual void OnUpdate() override;
		virtual void OnDestroy() override;
	private:
		bool _glfwInitialized = false;

		std::vector<Mem::Unique<Window>> _windowList;
	};
}