#include <Engine/Core/Engine.h>

#include <Engine/Window/WindowSubsystem.h>
#include <Engine/Renderer/RenderSubsystem.h>
#include <Engine/Subsystem/PluginLoaderSubsystem.h>
#include <Engine/MT/JobSubsystem.h>

#include "TestSystem.h"

int main() {
	{
		CusEngine::Engine engine{};
		engine.AddSubsystem<TestSubsystem>();
		engine.AddSubsystem<CusEngine::MT::JobSubsystem>();
		engine.AddSubsystem<CusEngine::PluginSubsystem>();
		engine.AddSubsystem<CusEngine::RenderSubsystem>();
		engine.AddSubsystem<CusEngine::WindowSubsystem>();
		engine.Run();
	}
	return 0;
}