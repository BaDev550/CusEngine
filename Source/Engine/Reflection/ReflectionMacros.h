#pragma once
#include <Engine/Reflection/TypeRegistry.h>

#define CUS_CLASS() 
#define CUS_PROP()

#define REFLECT_CLASS(Type) \
public: \
    virtual const CusEngine::Reflect::ClassType* GetTypeInfo() const override { \
        return CusEngine::Reflect::TypeRegistry::Get().GetClass(#Type); \
    } \
    static const CusEngine::Reflect::ClassType* StaticTypeInfo() { \
        return CusEngine::Reflect::TypeRegistry::Get().GetClass(#Type); \
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
            CusEngine::Reflect::TypeRegistry::Get().RegisterClass(typeInfo); \
        } \
        ~Type##_AutoRegister() { \
            CusEngine::Reflect::TypeRegistry::Get().UnregisterClass(#Type); \
        } \
    }; \
    Type##_AutoRegister global_##Type##_AutoRegister_Instance; \
    }