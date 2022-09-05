#pragma once

enum ComponentType
{
    CT_NONE = 0, //base zero
    CT_TRANSFORM, // Transform
    CT_MODEL,  //Graphics
    CT_MODEL_INSTANCE,
    CT_MODEL_BILLBOARD,
    CT_MODEL_TESSELLATION,
    CT_DIR_LIGHT, // Light
    CT_POINT_LIGHT,
    CT_SPOT_LIGHT,
    CT_CAMERA, //Camera
    CT_RIGID_BODY, //Physics
    CT_COLLISION, 
    CT_COUNT
};

// Common
class Transform;

// Graphic
class Model;
class ModelInstance;
class ModelBillboard;
class ModelTessellation;
class Sprite;
class SpriteText;
class DirLight;
class PointLight;
class SpotLight;
class Camera;

