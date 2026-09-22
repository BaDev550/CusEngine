#include <Engine/Core/Engine.h>

#include <Engine/Window/WindowSubsystem.h>
#include <Engine/Renderer/RenderSubsystem.h>
#include <Engine/Subsystem/PluginLoaderSubsystem.h>
#include <Engine/Reflection/ReflectionSubsystem.h>
#include <Engine/MT/JobSubsystem.h>
#include <Engine/Asset/AssetSubsystem.h>

#include <Runtime/IO/FileBuffer.h>

#include "TestSystem.h"

int main() {
	{
		CusEngine::Engine engine{};
		engine.AddSubsystem<TestSubsystem>();
		engine.AddSubsystem<CusEngine::PluginSubsystem>();
		engine.AddSubsystem<CusEngine::AssetSubsystem>();
		engine.AddSubsystem<CusEngine::Reflect::ReflectionSubsystem>();
		engine.AddSubsystem<CusEngine::MT::JobSubsystem>();
		engine.AddSubsystem<CusEngine::RenderSubsystem>();
		engine.AddSubsystem<CusEngine::WindowSubsystem>(); 
		engine.Run();
	}
	return 0;
}