#pragma once
#include "ReflectionMacros.h"
#include "Core/Object.h"

#include <glm/glm.hpp>

CUS_CLASS()
class TransformComponent : public CusEngine::Object {
    REFLECT_CLASS(TransformComponent)
public:
    CUS_PROP()
    glm::vec3 Position;

    CUS_PROP()
    float Rotation;
};