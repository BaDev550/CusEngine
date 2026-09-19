#pragma once

#include <Subsystem/Subsystem.h>
#include <Window/WindowSubsystem.h>
#include <Renderer/RenderSubsystem.h>

using namespace CusEngine;

class TestSubsystem final : public Subsystem {
public:
	virtual bool OnCreate(Engine* engine) override {
		Subsystem::OnCreate(engine);

		Logger::Info("TestSubsystem", "Created!");

		return true;
	}

	virtual void OnUpdate() override {}
	virtual void OnDestroy() override {}

	virtual void GetDependencyGraph(DependencyGraph& graph) {
		graph.Require<WindowSubsystem>(DependencyOrder::After);
		graph.Require<RenderSubsystem>(DependencyOrder::After);
	}
};