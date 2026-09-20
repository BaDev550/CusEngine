#pragma once
#include <Core/Object.h>

using namespace CusEngine;

CUS_CLASS()
class TestObject : public Object {
    REFLECT_CLASS(TestObject)
public:
    CUS_PROP()
    float MaxHealth = 100.0f;

    CUS_PROP()
    float CurrentHealth = 100.0f;
};