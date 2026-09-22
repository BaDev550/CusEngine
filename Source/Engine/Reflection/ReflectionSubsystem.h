#pragma once
#include <Engine/Core/Core.h>
#include <Engine/Core/Logger.h>
#include <Engine/Subsystem/Subsystem.h>
#include <Engine/Reflection/TypeDescriptor.h>

namespace CusEngine::Reflect {
    class ENGINE_API ReflectionSubsystem : public Subsystem {
    public:
        virtual Result OnCreate(Engine* engine) override;

        virtual void GetDependencyGraph(DependencyGraph& graph) override;

        void RegisterClass(const ClassType& typeInfo);
        void UnregisterClass(const std::string& className);

        const ClassType* GetClass(std::string_view className) const;

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