#include <Core/Engine.h>

#include <Window/WindowSubsystem.h>

#include "TestSystem.h"

int main() {
	{
		CusEngine::Engine engine{};
		engine.AddSubsystem<TestSubsystem>();
		engine.AddSubsystem<CusEngine::WindowSubsystem>();
		engine.Run();
	}
	return 0;
}