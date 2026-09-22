#pragma once
#include <Engine/Core/Engine.h>
#include <Engine/Reflection/ReflectionSubsystem.h>
#include <vector>

#define CUS_CLASS(...) 
#define CUS_PROP(...)

namespace CusEngine::Reflect {
    struct ClassType;
    ENGINE_API std::vector<ClassType>& GetPendingClasses(); // queue mybe baby
}

constexpr std::string_view className(std::string_view sig, bool fullQualified = false) {
    size_t endPos = sig.find("::StaticTypeInfo");
    if (endPos == std::string_view::npos) {
        endPos = sig.find("::GetTypeInfo");
    }
    if (endPos == std::string_view::npos) {
        return "";
    }

    std::string_view prefix = sig.substr(0, endPos);
    size_t lastSpace = prefix.rfind(' ');
    if (lastSpace != std::string_view::npos) {
        prefix = prefix.substr(lastSpace + 1);
    }
    if (prefix.starts_with("class "))   prefix.remove_prefix(6);
    if (prefix.starts_with("struct "))  prefix.remove_prefix(7);

    if (!fullQualified) {
        size_t lastColon = prefix.rfind("::");
        if (lastColon != std::string_view::npos) {
            prefix = prefix.substr(lastColon + 2);
        }
    }

    return prefix;
}

#if defined(_MSC_VER)
#define CUS_FUNC_SIG __FUNCSIG__
#else
#define CUS_FUNC_SIG __PRETTY_FUNCTION__
#endif
#define __CLASS_NAME__ className(CUS_FUNC_SIG)

#define REFLECT_CLASS() \
public: \
    virtual const CusEngine::Reflect::ClassType* GetTypeInfo() const override { \
        CusEngine::Reflect::ReflectionSubsystem* system = CusEngine::Engine::Get().GetSubsystem<CusEngine::Reflect::ReflectionSubsystem>(); \
        return system->GetClass(__CLASS_NAME__); \
    } \
    static std::string_view StaticTypeName() { return __CLASS_NAME__; }

#define BEGIN_REFLECT(Type) \
    using namespace CusEngine; \
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
            CusEngine::Reflect::GetPendingClasses().push_back(typeInfo); \
        } \
    }; \
    Type##_AutoRegister global_##Type##_AutoRegister_Instance; \
    }