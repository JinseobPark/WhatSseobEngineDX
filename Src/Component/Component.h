#pragma once
#include "ComponentType.h"

class Object;
class Model;

class Component
{
public:
    Component(ComponentType type) : cType(type) {}
    virtual ~Component() {}
    virtual void Initialize() {};
    void SetOwner(Object* owner) { pOwner = owner; }
    Object* GetOwner() { return pOwner; }
    ComponentType GetType() { return cType; }

private:
    friend class Model;
    friend class Object;
    Component() = delete;
    Object* pOwner = nullptr;
    ComponentType cType;
};