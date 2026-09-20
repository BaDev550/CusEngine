#pragma once
#include <Engine/Core/Object.h>

CUS_CLASS()
class HealthComponent : public CusEngine::Object {
    REFLECT_CLASS(HealthComponent)
public:
    CUS_PROP()
    float MaxHealth = 100.0f;

    CUS_PROP()
    float CurrentHealth = 100.0f;
};