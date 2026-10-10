#include <Engine/Plugin/PluginSystem.h>
#include <Engine/Window/WindowSubsystem.h>

#include <Runtime/Definitions/Logger.h>

#ifdef _WIN32
#include <windows.h>
#endif

namespace Tourqe::Engine {
	PluginSystem::PluginSystem() {

	}

    PluginSystem::~PluginSystem() {
        UnloadAll();
    }

    bool PluginSystem::LoadPlugin(const std::string& pluginName) {
#ifdef _WIN32
        HMODULE handle = LoadLibraryA(pluginName.c_str());
        if (handle) {
            _loadedPlugins.push_back(handle);
            Logger::Info("PluginSystem", "Successfully loaded: {}", pluginName);
            return true;
        }
        else {
            Logger::Info("PluginSystem", "Failed to load: {}", pluginName);
            return false;
        }
#else
        return false;
#endif
    }

    void PluginSystem::UnloadAll() {
        if (_loadedPlugins.size() > 0) {
            Logger::Info("PluginSystem", "Unloading {} plugins...", _loadedPlugins.size());
#ifdef _WIN32
            for (auto it = _loadedPlugins.rbegin(); it != _loadedPlugins.rend(); ++it) {
                FreeLibrary(static_cast<HMODULE>(*it));
            }
            _loadedPlugins.clear();
#endif
        }
    }
}