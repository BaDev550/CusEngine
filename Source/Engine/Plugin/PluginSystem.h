#pragma once
#include <string>
#include <vector>
#include <Engine/Core/Core.h>

namespace Tourqe::Engine {
    class ENGINE_API PluginSystem final {
    public:
        PluginSystem();
        ~PluginSystem();

        b8 LoadPlugin(const std::string& pluginName);
        void UnloadAll();
    private:
        std::vector<void*> _loadedPlugins;
    };

}