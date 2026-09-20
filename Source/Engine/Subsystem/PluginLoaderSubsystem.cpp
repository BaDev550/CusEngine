#include <Engine/Subsystem/PluginLoaderSubsystem.h>
#include <Engine/Window/WindowSubsystem.h>

#include <Engine/Core/Logger.h>

#ifdef _WIN32
#include <windows.h>
#endif

namespace CusEngine {
    bool PluginSubsystem::OnCreate(Engine* engine) {
        Subsystem::OnCreate(engine);

        return true;
    }

    void PluginSubsystem::OnUpdate() {}

    void PluginSubsystem::OnDestroy() {
        Logger::Info("PluginSubsystem", "Shutting down, unloading plugins...");
        UnloadAll();
    }

    bool PluginSubsystem::LoadPlugin(const std::string& pluginName) {
#ifdef _WIN32
        HMODULE handle = LoadLibraryA(pluginName.c_str());
        if (handle) {
            _loadedPlugins.push_back(handle);
            Logger::Info("PluginSubsystem", "Successfully loaded: {}", pluginName);
            return true;
        }
        else {
            Logger::Info("PluginSubsystem", "Failed to load: {}", pluginName);
            return false;
        }
#else
        return false;
#endif
    }

    void PluginSubsystem::UnloadAll() {
#ifdef _WIN32
        for (auto it = _loadedPlugins.rbegin(); it != _loadedPlugins.rend(); ++it) {
            FreeLibrary(static_cast<HMODULE>(*it));
        }
        _loadedPlugins.clear();
#endif
    }

    void PluginSubsystem::GetDependencyGraph(DependencyGraph& graph) {
        graph.Require<WindowSubsystem>(DependencyOrder::Before);
    }
}