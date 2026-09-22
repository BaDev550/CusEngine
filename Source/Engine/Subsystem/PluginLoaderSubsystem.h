#pragma once
#include <string>
#include <vector>
#include <Engine/Core/Core.h>
#include <Engine/Subsystem/Subsystem.h>

namespace CusEngine {
    class ENGINE_API PluginSubsystem final : public Subsystem {
    public:
        PluginSubsystem() = default;
        virtual ~PluginSubsystem() = default;

        virtual Result OnCreate(Engine* engine) override;
        virtual void OnUpdate() override;
        virtual void OnDestroy() override;

        bool LoadPlugin(const std::string& pluginName);
        void UnloadAll();

        virtual void GetDependencyGraph(DependencyGraph& graph) override;
    private:
        std::vector<void*> _loadedPlugins;
    };

}