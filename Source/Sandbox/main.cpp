#include <Core/Engine.h>

#include <Window/WindowSubsystem.h>
#include <Renderer/RenderSubsystem.h>
#include <MT/JobSubsystem.h>

#include "TestSystem.h"

int main() {
	{
		CusEngine::Engine engine{};
		engine.AddSubsystem<TestSubsystem>();
		engine.AddSubsystem<CusEngine::MT::JobSubsystem>();
		engine.AddSubsystem<CusEngine::RenderSubsystem>();
		engine.AddSubsystem<CusEngine::WindowSubsystem>();
		engine.Run();
	}
	return 0;
}