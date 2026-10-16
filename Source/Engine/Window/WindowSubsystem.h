#pragma once
#include <Runtime/Definitions/Types.h>
#include <Runtime/Memory/Memory.h>
#include <Runtime/PAL/Window/Window.h>
#include <Engine/Subsystem/Subsystem.h>
#include <vector>

namespace Tourqe::Engine {
	class ENGINE_API WindowSubsystem final : public Subsystem {
	public:
		virtual Runtime::Result OnCreate(Engine* engine) override;
		virtual void OnUpdate() override;
		virtual void OnDestroy() override;

		Runtime::PAL::Window* GetWindow() const { return _window; }
	private:
		Runtime::PAL::Window* _window = nullptr;
	};
}