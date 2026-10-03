#include <Engine/Core/Engine.h>

#include <Engine/Subsystem/PluginLoaderSubsystem.h>
#include <Engine/Renderer/RenderSubsystem.h>
#include <Engine/Window/WindowSubsystem.h>
#include <Engine/Asset/AssetSubsystem.h>
#include <Engine/Scene/SceneSubsystem.h>
#include <Engine/MT/JobSubsystem.h>

#include <LevelEditor/LevelEditor.h>

int main() {
	{
		CusEngine::Engine engine{};
		engine.AddSubsystem<CusEngine::PluginSubsystem>();
		engine.AddSubsystem<CusEngine::AssetSubsystem>();
		engine.AddSubsystem<CusEngine::MT::JobSubsystem>();
		engine.AddSubsystem<CusEngine::RenderSubsystem>();
		engine.AddSubsystem<CusEngine::WindowSubsystem>();
		engine.AddSubsystem<CusEngine::SceneSubsystem>();
		engine.AddSubsystem<LevelEditor>();
		engine.Run();
	}
	return 0;
}