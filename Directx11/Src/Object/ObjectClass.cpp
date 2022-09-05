#include "stdafx.h"
#include "ObjectClass.h"
#include "ObjectFactory.h"
#include "Systems/Application/Application.h"

Object::Object()
{
	m_transform = nullptr;
	m_model = nullptr;
    m_modelInstance = nullptr;
    m_modelBillboard = nullptr;
    m_modelTessellation = nullptr;

    m_dirLight = nullptr;
    m_pointLight = nullptr;
    m_spotLight = nullptr;

    m_camera = nullptr;

	m_objectID = 0;
	p_name = "";
}

Object::~Object()
{
    //release Components
	if (m_transform)
		delete m_transform;
	if (m_model)
		delete m_model;
    if (m_modelInstance)
        delete m_modelInstance;
    if (m_modelBillboard)
        delete m_modelBillboard;
    if (m_modelTessellation)
        delete m_modelTessellation;
    if (m_dirLight)
        delete m_dirLight;
    if (m_pointLight)
        delete m_pointLight;
    if (m_spotLight)
        delete m_spotLight;
    if (m_camera)
        delete m_camera;

    //clear name
	p_name.clear();
	p_name.shrink_to_fit();
}

void Object::Initialize()
{
    //initialize components
	if (m_transform)
		m_transform->Initialize();
    if (m_model)
        m_model->Initialize();
    if (m_modelInstance)
        m_modelInstance->Initialize();
    if (m_modelBillboard)
        m_modelBillboard->Initialize();
    if (m_modelTessellation)
        m_modelTessellation->Initialize();
    if (m_dirLight)
        m_dirLight->Initialize();
    if (m_pointLight)
        m_pointLight->Initialize();
    if (m_spotLight)
        m_spotLight->Initialize();
    if (m_camera)
        m_camera->Initialize();
}

void Object::Destroy(Object* obj)
{
	OBJECT_FACTORY->Destroy(obj);
}

bool Object::AddComponent(Component* pComponent)
{
    //All component and return true if success
    if (pComponent->GetType() > CT_NONE && pComponent->GetType() < CT_COUNT)
        pComponent->SetOwner(this);
    switch (pComponent->GetType())
    {
    case CT_TRANSFORM:
        m_transform = dynamic_cast<Transform*>(pComponent);
        return true;
    case CT_MODEL:
        m_model = dynamic_cast<Model*>(pComponent);
        return true;
    case CT_MODEL_INSTANCE:
        m_modelInstance = dynamic_cast<ModelInstance*>(pComponent);
        return true;
    case CT_MODEL_BILLBOARD:
        m_modelBillboard = dynamic_cast<ModelBillboard*>(pComponent);
        return true;
    case CT_MODEL_TESSELLATION:
        m_modelTessellation = dynamic_cast<ModelTessellation*>(pComponent);
        return true;
    case CT_DIR_LIGHT:
        m_dirLight = dynamic_cast<DirLight*>(pComponent);
        return true;
    case CT_POINT_LIGHT:
        m_pointLight = dynamic_cast<PointLight*>(pComponent);
        return true;
    case CT_SPOT_LIGHT:
        m_spotLight = dynamic_cast<SpotLight*>(pComponent);
        return true;
    case CT_CAMERA:
        m_camera = dynamic_cast<Camera*>(pComponent);
        return true;
        
    default: // Only working in debug mode
        assert(!"Allocating an unknown component in AddComponent!");
        break;
    }

    return false; 
}

Component* Object::GetComponent(ComponentType cType)
{
    switch (cType)
    {
    case CT_TRANSFORM:
        return m_transform;
    case CT_MODEL:
        return m_model;
    case CT_MODEL_INSTANCE:
        return m_modelInstance;
    case CT_MODEL_BILLBOARD:
        return m_modelBillboard;
    case CT_MODEL_TESSELLATION:
        return m_modelTessellation;
    case CT_DIR_LIGHT:
        return m_dirLight;
    case CT_POINT_LIGHT:
        return m_pointLight;
    case CT_SPOT_LIGHT:
        return m_spotLight;
    case CT_CAMERA:
        return m_camera;

    default:
        assert(!"Getting an unknown component in GetComponent!");
        break;
    }
    return nullptr;
}
