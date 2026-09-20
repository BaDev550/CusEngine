#pragma once
#include "Core/Core.h"
#include "Core/Logger.h"
#include "TypeDescriptor.h"

namespace CusEngine::Reflect {
    class ENGINE_API TypeRegistry {
    public:
        static TypeRegistry& Get();

        void RegisterClass(const ClassType& typeInfo);
        void UnregisterClass(const std::string& className);

        const ClassType* GetClass(const std::string& className) const;

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