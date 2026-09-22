#include "ReflectionSubsystem.h"
#include <Engine/Window/WindowSubsystem.h>
#include <Engine/Subsystem/PluginLoaderSubsystem.h>

namespace CusEngine::Reflect {
    std::vector<ClassType>& GetPendingClasses() {
        static std::vector<ClassType> pending;
        return pending;
    }

    Result ReflectionSubsystem::OnCreate(Engine* engine) {
        Subsystem::OnCreate(engine);

        for (auto& pclass : GetPendingClasses()) {
            RegisterClass(pclass);
            Logger::Info(GetTypeID().name(), "Registered class: {}", pclass.Name);
        }

        return Result();
    }

    void ReflectionSubsystem::RegisterClass(const ClassType& typeInfo) {
        _classes[typeInfo.Name] = typeInfo;
    }

    void ReflectionSubsystem::UnregisterClass(const std::string& className) {
        _classes.erase(className);
    }

    const ClassType* ReflectionSubsystem::GetClass(std::string_view name) const {
        auto it = _classes.find(name.data());
        if (it != _classes.end()) {
            return &it->second;
        }
        return nullptr;
    }

    void ReflectionSubsystem::GetDependencyGraph(DependencyGraph& graph) {
        graph.Require<PluginSubsystem>(DependencyOrder::Before);
        graph.Require<WindowSubsystem>(DependencyOrder::Before);
    }
}