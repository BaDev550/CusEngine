#pragma once

#include <Engine/Subsystem/Subsystem.h>
#include <Engine/Window/WindowSubsystem.h>
#include <Engine/Renderer/RenderSubsystem.h>

using namespace CusEngine;

class TestSubsystem final : public Subsystem {
public:
	virtual Result OnCreate(Engine* engine) override {
		Subsystem::OnCreate(engine);

		Logger::Info("TestSubsystem", "Created!");

		return Result();
	}

	virtual void OnUpdate() override {}
	virtual void OnDestroy() override {}

	virtual void GetDependencyGraph(DependencyGraph& graph) {
		graph.Require<WindowSubsystem>(DependencyOrder::After);
		graph.Require<RenderSubsystem>(DependencyOrder::After);
	}
};