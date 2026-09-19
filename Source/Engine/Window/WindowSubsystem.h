#pragma once
#include "Core/Types.h"
#include "Core/Memory.h"
#include "Window/Window.h"
#include "Subsystem/Subsystem.h"
#include <vector>

namespace CusEngine {
	class WindowSubsystem final : public Subsystem {
	public:
		virtual bool OnCreate(Engine* engine);
		virtual void OnUpdate();
		virtual void OnDestroy();

		virtual SubsystemOrder GetInitOrder() { return SubsystemOrder::After; } // TODO(0x): change this funcs to return a list
		virtual SubsystemOrder GetCreateOrder() { return SubsystemOrder::After; }
	private:
		bool _glfwInitialized = false;

		std::vector<Mem::Unique<Window>> _windowList;
	};
}