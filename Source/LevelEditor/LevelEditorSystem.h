#pragma once

#include <Engine/Subsystem/Subsystem.h>
#include "ContentBrowser.h"

class LevelEditorSubsystem final : public CusEngine::Subsystem {
public:
	virtual Runtime::Result OnCreate(CusEngine::Engine* engine) override;
	virtual void OnUpdate() override;
	virtual void OnDestroy() override;

	virtual void GetDependencyGraph(CusEngine::DependencyGraph& graph) override;
private:
	ContentBrowser _cb;
};