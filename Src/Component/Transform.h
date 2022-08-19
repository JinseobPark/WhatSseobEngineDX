#pragma once
#include "Component.h"
#include "stdafx.h"

class Transform : public Component
{
public:
    Transform() : Component(CT_TRANSFORM)
    {
        
        position = XMFLOAT3(0, 0, 0); 
        rotation = XMFLOAT3(0, 0, 0);  
        scale = XMFLOAT3(1, 1, 1); 
    }
    Transform(const Transform& trans) : Component(CT_TRANSFORM)
    {
        position = trans.position;
        rotation = trans.rotation;
        scale = trans.scale;
    }

    const XMFLOAT3& getPosition() const { return position; }
    const XMFLOAT3& getRotation() const { return rotation; }
    const XMFLOAT3& getScale() const { return scale; }
    void setPosition(const XMFLOAT3& pos) { position = pos; }
    void setPosition(const float x, const float y, const float z) { position = XMFLOAT3(x, y, z); }
    void setRotation(const XMFLOAT3& rot) { rotation = rot; }
    void setRotation(const float x, const float y, const float z) { rotation = XMFLOAT3(x, y, z); }
    void setScale(const XMFLOAT3& s) { scale = s; }
    void setScale(const float x, const float y, const float z) { scale = XMFLOAT3(x, y, z); }

    XMFLOAT3 position;
    XMFLOAT3 rotation;
    XMFLOAT3 scale;
};