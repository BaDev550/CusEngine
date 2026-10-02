#include <Engine/Core/Engine.h>

#include <Engine/Window/WindowSubsystem.h>
#include <Engine/Renderer/RenderSubsystem.h>
#include <Engine/Scene/Systems/SceneRenderer2D.h>
#include <Engine/Subsystem/PluginLoaderSubsystem.h>
#include <Engine/Reflection/ReflectionSubsystem.h>
#include <Engine/MT/JobSubsystem.h>
#include <Engine/Asset/AssetSubsystem.h>

#include "LevelEditorSystem.h"

int main() {
	{
		CusEngine::Engine engine{};
		engine.AddSubsystem<CusEngine::Reflect::ReflectionSubsystem>();
		engine.AddSubsystem<CusEngine::MT::JobSubsystem>();
		engine.AddSubsystem<CusEngine::PluginSubsystem>();
		engine.AddSubsystem<CusEngine::AssetSubsystem>();
		engine.AddSubsystem<CusEngine::RenderSubsystem>();
		engine.AddSubsystem<CusEngine::WindowSubsystem>(); 
		engine.AddSubsystem<CusEngine::SceneRenderer2DSubsystem>();
		engine.AddSubsystem<LevelEditorSubsystem>();
		engine.Run();
	}
	return 0;
}