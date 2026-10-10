#pragma once

#include <Engine/Subsystem/Subsystem.h>

class Console final {
public:
	void Draw();
private:
	void ProcessCommand(std::string_view command);
	void Log(std::string_view msg);

	std::vector<std::string> _history;
	bool _scrollToBottom = false;
};

class LevelEditor final : public Tourqe::Engine::Subsystem {
public:
	virtual Runtime::Result OnCreate(Tourqe::Engine::Engine* engine) override;
	virtual void OnUpdate() override;
	virtual void OnDestroy() override;
	virtual void GetDependencyGraph(Tourqe::Engine::DependencyGraph& graph) override;
private:
	Console console;
};