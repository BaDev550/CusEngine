#include <Engine/Core/Engine.h>

#include <Engine/Renderer/RenderSubsystem.h>
#include <Engine/Window/WindowSubsystem.h>
#include <Engine/Asset/AssetSubsystem.h>
#include <Engine/Scene/SceneSubsystem.h>

#include <LevelEditor/LevelEditor.h>

int main() {
	{
		Tourqe::Engine::Engine engine{};
		engine.AddSubsystem<Tourqe::Engine::AssetSubsystem>();
		engine.AddSubsystem<Tourqe::Engine::RenderSubsystem>();
		engine.AddSubsystem<Tourqe::Engine::WindowSubsystem>();
		//engine.AddSubsystem<Tourqe::Engine::SceneSubsystem>();
		engine.AddSubsystem<LevelEditor>();
		engine.Run();
	}
	return 0;
}