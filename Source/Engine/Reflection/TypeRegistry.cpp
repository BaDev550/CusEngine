#include "TypeRegistry.h"

namespace CusEngine::Reflect {
    TypeRegistry& TypeRegistry::Get() {
        static TypeRegistry instance;
        return instance;
    }

    void TypeRegistry::RegisterClass(const ClassType& typeInfo) {
        _classes[typeInfo.Name] = typeInfo;
    }

    void TypeRegistry::UnregisterClass(const std::string& className) {
        _classes.erase(className);
    }

    const ClassType* TypeRegistry::GetClass(const std::string& name) const {
        auto it = _classes.find(name);
        if (it != _classes.end()) {
            return &it->second;
        }
        return nullptr;
    }
}