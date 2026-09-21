#pragma once
#include <Engine/Core/Engine.h>
#include <Engine/Reflection/ReflectionSubsystem.h>

#define CUS_CLASS() 
#define CUS_PROP()

#define REFLECT_CLASS(Type) \
public: \
    virtual const CusEngine::Reflect::ClassType* GetTypeInfo() const override { \
        CusEngine::Reflect::ReflectionSubsystem* system = CusEngine::Engine::Get().GetSubsystem<CusEngine::Reflect::ReflectionSubsystem>(); \
        return system->GetClass(#Type); \
    } \
    static const CusEngine::Reflect::ClassType* StaticTypeInfo() { \
        CusEngine::Reflect::ReflectionSubsystem* system = CusEngine::Engine::Get().GetSubsystem<CusEngine::Reflect::ReflectionSubsystem>(); \
        return system->GetClass(#Type); \
    }

#define BEGIN_REFLECT(Type) \
    namespace { \
        struct Type##_AutoRegister { \
            Type##_AutoRegister() { \
                CusEngine::Reflect::ClassType typeInfo; \
                typeInfo.Name = #Type; \
                typeInfo.Size = sizeof(Type); \
                typeInfo.Instantiate = []() -> void* { return new Type(); }; 

#define REFLECT_PROPERTY(Type, VarName) \
                typeInfo.Properties.push_back( \
                    CusEngine::Reflect::Property::Bind(#VarName, &Type::VarName) \
                );

#define END_REFLECT(Type) \
            CusEngine::Reflect::ReflectionSubsystem* system = CusEngine::Engine::Get().GetSubsystem<CusEngine::Reflect::ReflectionSubsystem>(); \
            system->RegisterClass(typeInfo); \
        } \
        ~Type##_AutoRegister() { \
            system->UnregisterClass(#Type); \
        } \
    }; \
    Type##_AutoRegister global_##Type##_AutoRegister_Instance; \
    }