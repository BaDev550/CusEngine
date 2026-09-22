#pragma once
#include <Engine/Core/Types.h>
#include <Engine/Core/Memory.h>
#include <Engine/Window/Window.h>
#include <Engine/Subsystem/Subsystem.h>
#include <vector>

namespace CusEngine {
	class ENGINE_API WindowSubsystem final : public Subsystem {
	public:
		virtual Result OnCreate(Engine* engine) override;
		virtual void OnUpdate() override;
		virtual void OnDestroy() override;

		Window* GetWindow() const { return _window; }
	private:
		bool _glfwInitialized = false;

		Window* _window = nullptr;
	};
}