#pragma once
#include <Engine/Core/Core.h>
#include <Runtime/Definitions/Logger.h>
#include <Engine/Subsystem/Subsystem.h>
#include <Engine/Reflection/TypeDescriptor.h>

namespace CusEngine::Reflect {
    class ENGINE_API ReflectionSubsystem : public Subsystem {
    public:
        virtual Runtime::Result OnCreate(Engine* engine) override;

        virtual void GetDependencyGraph(DependencyGraph& graph) override;

        void RegisterClass(const ClassType& typeInfo);
        void UnregisterClass(const std::string& className);

        const ClassType* GetClass(std::string_view className) const;

        template<typename T>
		std::vector<const ClassType*> GetClassesByBase() const {
			std::vector<const ClassType*> result;

			const ClassType* baseClassType = GetClass(T::StaticClassName());
			if (!baseClassType) {
				return result;
			}

			for (const auto& [name, c] : _classes) {
				if (&c == baseClassType) {
					continue;
				}

				if (c.IsA(baseClassType)) {
					result.push_back(&c);
				}
			}
            
			return result;
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