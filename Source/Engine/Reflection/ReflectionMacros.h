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
    size_t paramPos = sig.rfind('(');
    if (paramPos != std::string_view::npos) {
        sig = sig.substr(0, paramPos);
    }

    size_t funcColons = sig.rfind("::");
    if (funcColons == std::string_view::npos) {
        return "";
    }
    sig = sig.substr(0, funcColons);

    int bracketDepth = 0;
    size_t startPos = 0;

    for (size_t i = sig.length(); i > 0; --i) {
        char c = sig[i - 1];
        if (c == '>') {
            bracketDepth++;
        }
        else if (c == '<') {
            bracketDepth--;
        }
        else if (bracketDepth == 0) {
            if (c == ' ' || c == '\t') {
                startPos = i;
                break;
            }
        }
    }

    std::string_view fullName = sig.substr(startPos);

    if (fullName.starts_with("class "))   fullName.remove_prefix(6);
    if (fullName.starts_with("struct "))  fullName.remove_prefix(7);

    if (fullQualified) {
        return fullName;
    }

    bracketDepth = 0;
    size_t lastColon = std::string_view::npos;

    for (size_t i = fullName.length(); i > 0; --i) {
        char c = fullName[i - 1];
        if (c == '>') {
            bracketDepth++;
        }
        else if (c == '<') {
            bracketDepth--;
        }
        else if (bracketDepth == 0 && c == ':') {
            if (i > 1 && fullName[i - 2] == ':') {
                lastColon = i;
                break;
            }
        }
    }

    if (lastColon != std::string_view::npos) {
        return fullName.substr(lastColon);
    }

    return fullName;
}

#if defined(_MSC_VER)
#define CUS_FUNC_SIG __FUNCSIG__
#else
#define CUS_FUNC_SIG __PRETTY_FUNCTION__
#endif

#define REFLECT_CLASS() \
public: \
    static constexpr std::string_view StaticClassName() { return className(CUS_FUNC_SIG, false); } \
    static constexpr std::string_view StaticFullClassName() { return className(CUS_FUNC_SIG, true); }

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