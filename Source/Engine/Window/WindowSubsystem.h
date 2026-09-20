#pragma once
#include "Core/Types.h"
#include "Core/Memory.h"
#include "Window/Window.h"
#include "Graphics/RHI/RHI_RenderContext.h"
#include "Subsystem/Subsystem.h"
#include <vector>

namespace CusEngine {
	class ENGINE_API WindowSubsystem final : public Subsystem {
	public:
		virtual bool OnCreate(Engine* engine) override;
		virtual void OnUpdate() override;
		virtual void OnDestroy() override;

		Window* GetWindow() const { return _window.get(); }

		virtual void GetDependencyGraph(DependencyGraph& graph) override;
	private:
		bool _glfwInitialized = false;

		Mem::Unique<Window> _window = nullptr;
	};
}