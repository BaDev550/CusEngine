#include "ReflectionSubsystem.h"
#include <Engine/Window/WindowSubsystem.h>
#include <Engine/Subsystem/PluginLoaderSubsystem.h>

namespace CusEngine::Reflect {
    void ReflectionSubsystem::RegisterClass(const ClassType& typeInfo) {
        _classes[typeInfo.Name] = typeInfo;
    }

    void ReflectionSubsystem::UnregisterClass(const std::string& className) {
        _classes.erase(className);
    }

    const ClassType* ReflectionSubsystem::GetClass(const std::string& name) const {
        auto it = _classes.find(name);
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