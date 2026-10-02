#include "LevelEditorSystem.h"
#include <Engine/Renderer/RenderSubsystem.h>

#include <imgui.h>

Runtime::Result LevelEditorSubsystem::OnCreate(CusEngine::Engine* engine) {
    Subsystem::OnCreate(engine);
	auto renderSystem = CusEngine::Engine::Get()->GetSubsystem<CusEngine::RenderSubsystem>();

    ImGui::SetCurrentContext(renderSystem->GetImGuiContext());

    return Runtime::Result();
}

void LevelEditorSubsystem::OnUpdate() {
    auto renderSystem = CusEngine::Engine::Get()->GetSubsystem<CusEngine::RenderSubsystem>();

    renderSystem->DrawImGui([=]() {
        ImGui::Begin("Debug");
        
        

        ImGui::End();

        _cb.DrawWindow();

        });
}

void LevelEditorSubsystem::OnDestroy() {}

void LevelEditorSubsystem::GetDependencyGraph(CusEngine::DependencyGraph & graph) {
    graph.Require<CusEngine::RenderSubsystem>(CusEngine::DependencyOrder::After);
}