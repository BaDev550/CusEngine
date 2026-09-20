#pragma once
#include "Core/Logger.h"
#include "TypeDescriptor.h"

namespace CusEngine::Reflect {
    class TypeRegistry {
    public:
        static TypeRegistry& Get() {
            static TypeRegistry instance;
            return instance;
        }

        void RegisterClass(const ClassType& typeInfo) {
            _classes[typeInfo.Name] = typeInfo;
        }

        const ClassType* GetClass(const std::string& className) const {
            auto it = _classes.find(className);
            return it != _classes.end() ? &it->second : nullptr;
        }

        template<typename T = void>
        T* Create(const std::string& className) {
            const ClassType* type = GetClass(className);
            if (!type || !type->Instantiate) {
                Logger::Error("Reflection", "Cannot instantiate: {}", className);
                return nullptr;
            }
            return static_cast<T*>(type->Instantiate());
        }

    private:
        std::unordered_map<std::string, ClassType> _classes;
    };
}