#include <Core/Engine.h>

#include <Window/WindowSubsystem.h>

#include "TestSystem.h"

int main() {
	{
		CusEngine::Engine engine{};
		engine.AddSubsystem<CusEngine::WindowSubsystem>();
		engine.AddSubsystem<TestSubsystem>();
		engine.Run();
	}
	return 0;
}