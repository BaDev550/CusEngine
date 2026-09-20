#include "Reflection/TransformComponent.h"
#include "Reflection/ReflectionMacros.h"

BEGIN_REFLECT(TransformComponent)
    REFLECT_PROPERTY(TransformComponent, Position)
    REFLECT_PROPERTY(TransformComponent, Rotation)
END_REFLECT(TransformComponent)
